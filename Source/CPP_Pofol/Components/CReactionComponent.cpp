#include "Components/CReactionComponent.h"
#include "Actors/Character/CCharacter.h"
#include "CGameInstance.h"
#include "DamageType/CDamageType.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Enums/CESkillType.h"
#include "Util/CLog.h"
#include "Kismet/KismetMathLibrary.h"


UCReactionComponent::UCReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve_RimRight(TEXT("CurveFloat'/Game/Curves/Curve_Hit_RimRight.Curve_Hit_RimRight'"));
	CurveFloat = Curve_RimRight.Object;
}


void UCReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	//Set Property
	{
		owner = Cast<ACCharacter>(GetOwner());
		check(owner);
		gameInstance = Cast<UCGameInstance>(GetWorld()->GetGameInstance());
		check(gameInstance);
	}

	//Set Delegate
	{
		//owner->OnTakeAnyDamage.AddDynamic(this, &UCReactionComponent::TakeDamage);
		//owner->OnTakeRadialDamage.AddDynamic(this, &UCReactionComponent::TakeRadialDamage);
		owner->OnTakePointDamage.AddDynamic(this, &UCReactionComponent::TakePointDamage);
	}

	//Set TimeLine for RimRight
	{
		if (CurveFloat)
		{
			FOnTimelineFloat TimelineProgress;
			TimelineProgress.BindUFunction(this, FName("DoingRimRight"));
			CurveTimeline.AddInterpFloat(CurveFloat, TimelineProgress);
		}

		for (int i = 0; i < owner->GetMesh()->GetNumMaterials(); ++i)
		{
			ownerMaterials.Emplace(owner->GetMesh()->CreateAndSetMaterialInstanceDynamic(i));
		}

		curveHalfLength = CurveTimeline.GetTimelineLength() / 2.0f;
	}
}


void UCReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurveTimeline.IsPlaying() == true || CurveTimeline.IsReversing() == true)
	{
		CurveTimeline.TickTimeline(GetWorld()->GetDeltaSeconds());
	}

	if (bDoUpdateShake == true)
	{
		DoingShakeActor();
	}
}


void UCReactionComponent::TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	hittingActor = Cast<ACharacter>(DamageCauser);
	if (hittingActor == nullptr) return;

	/**	Reaction������ ���� ó���� �ϰ� ���� �������θ� ��ȯ */
	//CDO�� ���� ���������� ���� ������ const_cast���
	const UCDamageType * tempDamageType = const_cast<UCDamageType*>(Cast<UCDamageType>(DamageType));
	if (ReactionHandle(tempDamageType->reactionType, tempDamageType->knockBackPower) == false) return;

	if (OnHitted.IsBound() == true) OnHitted.Execute(tempDamageType->reactionType);

	KnockBackActor_Forward(tempDamageType->knockBackPower.X);

	/**	RimRight ó�� */
	StartRimRight();

	/**	����(Hit Stop) ó�� */
	if (FMath::IsNearlyZero(tempDamageType->stiffness) == false)
	{
		StartShakeActor();
		gameInstance->RequestAdjustTime(tempDamageType->stiffness, 0.0f, owner);
	}

	

	/**	�´� ��ƼŬ ���� */
	for (int i = 0;  i < tempDamageType->useParticleToEnemy.Num(); ++i)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			tempDamageType->useParticleToEnemy[i],
			HitLocation,
			ShotFromDirection.Rotation()
		);
	}
}

/*********************************************************************************************
	* Shake Actor
********************************************************************************************* */

void UCReactionComponent::StartShakeActor()
{
	bDoUpdateShake = true;
	originRelativeLoc = owner->GetMesh()->GetRelativeLocation();
	this->SetComponentTickEnabled(true);
}

void UCReactionComponent::DoingShakeActor()
{
	owner->GetMesh()->SetRelativeLocation(originRelativeLoc + (FMath::VRand() * shakeAmount));
}

void UCReactionComponent::EndShakeActor()
{
	if (bDoUpdateShake == false) return;

	bDoUpdateShake = false;
	owner->GetMesh()->SetRelativeLocation(originRelativeLoc);
}


/*********************************************************************************************
	* Rim Right
********************************************************************************************* */

void UCReactionComponent::StartRimRight()
{
	/**	�� ����Ʈ */
	if (CurveTimeline.IsPlaying() == true || CurveTimeline.IsReversing() == true)
	{
		if (CurveTimeline.GetPlayRate() < curveHalfLength)
		{
			CurveTimeline.Play();
		}
		else
		{
			CurveTimeline.Reverse();
		}
	}
	else
	{
		CurveTimeline.PlayFromStart();
	}
}

void UCReactionComponent::DoingRimRight(float value)
{
	for (UMaterialInstanceDynamic * data : ownerMaterials)
	{
		data->SetScalarParameterValue(FName("Fresnel_Alpha"), value);
	}

}


/*********************************************************************************************
	* Knock Back
********************************************************************************************* */

bool UCReactionComponent::ReactionHandle(const EReactionType & reactionType, const FVector & knockBackAmount)
{
	bool canHittedBottom = CheckCanHittedBottom();
	return canHittedBottom;

	if(owner->GetMainState() == EMainState::AIR)
	{
		if (reactionType == EReactionType::SMASH_DOWN)
		{
			KnockBackActor_Upper(-3000.f);
		}
		else
		{
			KnockBackActor_Upper(250.f);
			SetTimerForGravityInAirState();
		}
	}
	if (owner->GetMainState() == EMainState::GROUND)
	{
		if (reactionType == EReactionType::SMASH_UPPER)
		{
			KnockBackActor_Upper(knockBackAmount.Z);
			SetTimerForGravityInAirState();
		}
		if (reactionType == EReactionType::STRONG)
		{
			owner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(owner->GetActorLocation(), hittingActor->GetActorLocation()));
		}
	}
}

void UCReactionComponent::KnockBackActor_Forward(float forwardAmount)
{
	FVector tempNorm = (owner->GetActorLocation() - hittingActor->GetActorLocation()).GetSafeNormal();
	owner->AddActorWorldOffset(tempNorm * forwardAmount);
}

void UCReactionComponent::KnockBackActor_Upper(float upAmount)
{
	owner->GetCharacterMovement()->StopMovementImmediately();
	owner->LaunchCharacter(FVector(0, 0, upAmount), false, true);
}

void UCReactionComponent::SetTimerForGravityInAirState()
{
	/**	||������ �߷� ���� Ÿ�̸Ӱ� ����Ǿ����� Ȯ���Ͽ� �������̶�� �����ŵ�ϴ�. || */
	if (GetWorld()->GetTimerManager().IsTimerActive(gravityHandle) == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(gravityHandle);
	}

	/**	||�߷��� ���� �����ϰ� Ÿ�̸Ӹ� �����ŵ�ϴ�. || */
	owner->GetCharacterMovement()->GravityScale = upperGravity;
	GetWorld()->GetTimerManager().SetTimer(gravityHandle, this, 
		&UCReactionComponent::SetOriginGravity, GetWorld()->GetDeltaSeconds(), true);
}

void UCReactionComponent::SetOriginGravity()
{
	if (owner->GetVelocity().Z < 10.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(gravityHandle);
		owner->GetCharacterMovement()->GravityScale = 3.0f;
	}
}

bool UCReactionComponent::CheckCanHittedBottom()
{
	float hittingActorLocZ = hittingActor->GetMesh()->GetBoneLocation(FName("pelvis")).Z;
	float ownerLocZ = owner->GetMesh()->GetBoneLocation(centerBone).Z;

	return ownerLocZ < hittingActorLocZ ? true : false;
}


//void UCReactionComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
//{
//	//CDO�� ���� ���������� ���� ������ const_cast���
//	const UCDamageType * tempDamageType = const_cast<UCDamageType*>(Cast<UCDamageType>(DamageType));
//	gameInstance->RequestAdjustTime(tempDamageType->stiffness, 0.0f, owner);
//	CLog::Log("Take Damage!!");
//}

//�̰� ���� �Ⱦ�
//void UCReactionComponent::TakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
//{
//	//if (OnHitted.IsBound() == true) OnHitted.Execute();
//
//	//CDO�� ���� ���������� ���� ������ const_cast���
//	/*const UCDamageType * tempDamageType = const_cast<UCDamageType*>(Cast<UCDamageType>(DamageType));
//	gameInstance->RequestAdjustTime(tempDamageType->stiffness, 0.0f, owner);
//
//	damagedActorLoc = DamageCauser->GetActorLocation();
//
//	StartShakeActor();*/
//}
