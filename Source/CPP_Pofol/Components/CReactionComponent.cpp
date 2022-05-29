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

	/**	Reaction유형에 따른 처리를 하고 공격 성공여부를 반환 */
	//CDO를 절대 변경할일이 없이 때문에 const_cast사용
	const UCDamageType * tempDamageType = const_cast<UCDamageType*>(Cast<UCDamageType>(DamageType));
	EReactionType finalReaction = ReactionHandle(tempDamageType->reactionType, tempDamageType->knockBackPower);
	if (finalReaction == EReactionType::NONE) return;
	if (OnHitted.IsBound() == true) OnHitted.Execute(finalReaction);


	KnockBackActor_Forward(tempDamageType->knockBackPower.X);

	/**	RimRight 처리 */
	StartRimRight();

	/**	경직(Hit Stop) 처리 */
	if (FMath::IsNearlyZero(tempDamageType->stiffness) == false)
	{
		StartShakeActor();
		gameInstance->RequestAdjustTime(tempDamageType->stiffness, 0.0f, owner);
	}

	

	/**	맞는 파티클 생성 */
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
	/**	림 라이트 */
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

EReactionType UCReactionComponent::ReactionHandle(const EReactionType & reactionType, const FVector & knockBackAmount)
{
	/**	||공중인지 땅에있는지 확인|| */
	if (owner->GetMainState() == EMainState::AIR)
	{
		SetHittedState_Air();
	}
	else if (owner->GetSubState() == ESubState::LAY_DOWN)
	{
		SetHittedState_Ground();
	}
	//CLog::ScreenEnumLog("ReactionType : ", GetHittedState(), FColor::Green, 2.f);


	/**	||상태와 상관없이 항상 똑같은 처리를 하는 리액션 타입의 처리|| */
	if (reactionType == EReactionType::SMASH_UPPER)
	{
		SetHittedState_Air();
		KnockBackActor_Upper(knockBackAmount.Z);
		SetTimerForGravityInAirState();
		return EReactionType::SMASH_UPPER;
	}
	if (reactionType == EReactionType::SMASH_DOWN)
	{
		SetHittedState_Ground();
		KnockBackActor_Upper(-3000.f);
		return EReactionType::SMASH_DOWN;
	}
	if (reactionType == EReactionType::STRONG)
	{
		float tempYaw = UKismetMathLibrary::FindLookAtRotation(owner->GetActorLocation(), hittingActor->GetActorLocation()).Yaw;
		owner->SetActorRotation(FRotator(0.f, tempYaw, 0.f));
		return EReactionType::STRONG;
	}

	bool canHittedBottom = CheckCanHittedBottom();
	if (GetHittedState() == EHittedState::AIR)
	{///공중에 떠있다면
		KnockBackActor_Upper(300.f);
		SetTimerForGravityInAirState();
		return EReactionType::SMASH_UPPER;
	}
	if (owner->GetSubState() == ESubState::LAY_DOWN)
	{///누워있다면
		return EReactionType::LAY_DOWN_HIT;
	}
	if (GetHittedState() == EHittedState::GROUND)
	{///서있다면
		return EReactionType::NORMAL;
	}

	return EReactionType::NONE;
	


	//if(owner->GetMainState() == EMainState::AIR)
	//{
	//	KnockBackActor_Upper(250.f);
	//	SetTimerForGravityInAirState();
	//}
	//if (owner->GetMainState() == EMainState::GROUND)
	//{
	//	
	//	if (reactionType == EReactionType::STRONG)
	//	{
	//		owner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(owner->GetActorLocation(), hittingActor->GetActorLocation()));
	//	}
	//}
}

void UCReactionComponent::KnockBackActor_Forward(float forwardAmount)
{
	FVector tempNorm = (owner->GetActorLocation() - hittingActor->GetActorLocation()).GetSafeNormal2D();
	owner->AddActorWorldOffset(tempNorm * forwardAmount);
}

void UCReactionComponent::KnockBackActor_Upper(float upAmount)
{
	owner->GetCharacterMovement()->StopMovementImmediately();
	owner->LaunchCharacter(FVector(0, 0, upAmount), false, false);
}

void UCReactionComponent::SetTimerForGravityInAirState()
{
	/**	||이전에 중력 조절 타이머가 실행되었는지 확인하여 실행중이라면 종료시킵니다. || */
	if (GetWorld()->GetTimerManager().IsTimerActive(gravityHandle) == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(gravityHandle);
	}

	/**	||중력을 낮게 조정하고 타이머를 실행시킵니다. || */
	owner->GetCharacterMovement()->GravityScale = upperGravity;
	GetWorld()->GetTimerManager().SetTimer(gravityHandle, this, 
		&UCReactionComponent::SetOriginGravity, GetWorld()->GetDeltaSeconds(), true);
}

void UCReactionComponent::SetOriginGravity()
{
	if (owner->GetVelocity().Z < -400.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(gravityHandle);
		owner->GetCharacterMovement()->GravityScale = 1.f;
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
//	//CDO를 절대 변경할일이 없이 때문에 const_cast사용
//	const UCDamageType * tempDamageType = const_cast<UCDamageType*>(Cast<UCDamageType>(DamageType));
//	gameInstance->RequestAdjustTime(tempDamageType->stiffness, 0.0f, owner);
//	CLog::Log("Take Damage!!");
//}

//이거 아직 안씀
//void UCReactionComponent::TakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
//{
//	//if (OnHitted.IsBound() == true) OnHitted.Execute();
//
//	//CDO를 절대 변경할일이 없이 때문에 const_cast사용
//	/*const UCDamageType * tempDamageType = const_cast<UCDamageType*>(Cast<UCDamageType>(DamageType));
//	gameInstance->RequestAdjustTime(tempDamageType->stiffness, 0.0f, owner);
//
//	damagedActorLoc = DamageCauser->GetActorLocation();
//
//	StartShakeActor();*/
//}
