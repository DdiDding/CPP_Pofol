#include "CWeaponManagerComponent.h"
#include "CSkillManagerComponent.h"
#include "DataAssets/CDA_WeaponData.h"
#include "Actors/Character/CCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enums/CESkillType.h"
#include "Util/CLog.h"

UCWeaponManagerComponent::UCWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UCWeaponManagerComponent::~UCWeaponManagerComponent()
{
	/*for (int i = 0; i < usingWeaponStaticMeshes.Num(); ++i)
	{
		usingWeaponStaticMeshes[i]->UnregisterComponent();
		usingWeaponStaticMeshes[i]->DestroyComponent();
	}

	for (int i = 0; i < niagaraTempStaticMeshes.Num(); ++i)
	{
		niagaraTempStaticMeshes[i]->UnregisterComponent();
		niagaraTempStaticMeshes[i]->DestroyComponent();
	}*/
}


void UCWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	owner = Cast<ACCharacter>(GetOwner());
	if (owner != nullptr)
	{
		UCSkillManagerComponent * smc = Cast<UCSkillManagerComponent>(owner->GetComponentByClass(UCSkillManagerComponent::StaticClass()));
		if (smc != nullptr)
		{
			smc->OnSpawnWeaponDelegate.BindUFunction(this, FName("SpawnWeapon"));
			smc->OnResetHitArrayDelegate.BindUFunction(this, FName("ResetHitsEmeny"));
			smc->OnEndSkill.BindUFunction(this, FName("UnSpawnWeapon"));
		}
	}

	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("SpawnWeaponMaterial"));
		CurveTimeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	//Niagara setting why crush??
	{
		/*static ConstructorHelpers::FObjectFinder<UNiagaraSystem> UnSpawnEffect(TEXT("NiagaraSystem'/Game/Resources/Particles/Weapons/WeaponDistroy/NS_UnSpawnWeapon.NS_UnSpawnWeapon'"));
		unSpawnEffect = UnSpawnEffect.Object;*/
	}
}


void UCWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurveTimeline.IsPlaying() == true || CurveTimeline.IsReversing() == true)
	{
		CurveTimeline.TickTimeline(DeltaTime);
	}

	if (bDoTrace == true)
	{
		UpdateTrace_PointDamage();
	}

	DEBUG_WEAPON{ Debug(); }
}

void UCWeaponManagerComponent::UpdateTrace_PointDamage()
{
	if (currentWeapon == nullptr)
	{
		return;
	}

	//무기개수 만큼
	int weaponNum = 0;
	for (FWeaponData & data : currentWeapon->weaponData)
	{
		if (usingWeaponStaticMeshes.Num() == 0)
		{
			DEBUG_WEAPON{ CLog::Log(L"usingWeaponStaticMeshes is Nullptr!!"); }
			++weaponNum;
			return;
		}

		//소켓개수만큼
		TArray<FHitResult> tempHits;
		for (int i = 0; i < data.traceSocketCnt; i++)
		{
			
			FVector endLoc = usingWeaponStaticMeshes[weaponNum]->GetSocketLocation(*FString::FromInt(i));
			FVector startLoc;
			if (traceSocketLoc[weaponNum].locArray[i] == FVector::ZeroVector) {	startLoc = endLoc; }
			else { startLoc = traceSocketLoc[weaponNum].locArray[i]; }
			
			//MeleeAttack 콜리전 채널로 쏜다.
			if (GetWorld()->LineTraceMultiByChannel(tempHits, startLoc, endLoc,
				ECollisionChannel::ECC_GameTraceChannel3) == true)
			{
				for (FHitResult & hit : tempHits)
				{
					if (hit.GetActor() != owner)
					{///자기 자신이 아니면 진행
						if (hitsEnemy.Find(hit.GetActor()) == INDEX_NONE)
						{///이미 맞은 적이 아니라면 저장
							CLog::Log(hit.GetActor()->GetName(),"Hit!! : ");
							
							hitsEnemy.Emplace(hit.GetActor());
							if (OnDamageDelegate.IsBound() == true) OnDamageDelegate.Execute(hit, EAttackType::POINT_DAMAGE);
						}
					}
				}
			}

			DEBUG_WEAPON { DrawDebugLine(GetWorld(), startLoc, endLoc,	FColor::Red, false, 0.5f, 0, arrowThick); }
			traceSocketLoc[weaponNum].locArray[i] = endLoc;
		}

		//traceSocketLoc.Init(FVector::ZeroVector, traceSocketLoc.Num());
		++weaponNum;
	}
}


void UCWeaponManagerComponent::UpdateTrace_MeleeRangeDamage(FVector addLoc, float radius, float dotRange)
{
	/** || Spherer형 트레이스를 쏘아 반경 radius안에 있는 적을 찾아 내적을 통해 전방에 있는 적에게 데미지 이벤트를 실행 || */
	TArray<FHitResult> tempHits;
	FVector finalLoc = owner->GetActorLocation() + addLoc;
	if (UKismetSystemLibrary::SphereTraceMulti(GetWorld(), finalLoc, finalLoc,
		radius, ETraceTypeQuery::TraceTypeQuery4, false, TArray<AActor*>(), EDrawDebugTrace::None, tempHits, true) == false)
	{
		return;
	}

	for (FHitResult& data : tempHits)
	{
		if ( dotRange <= FVector::DotProduct(
			owner->GetActorForwardVector(),
			(data.GetActor()->GetActorLocation() - owner->GetActorLocation()).GetSafeNormal2D() ))
		{///캐릭터 전방에 있는 적만 데미지 이벤트 실행
			CLog::Log(data.GetActor()->GetName(), "Hit range! : ");
			if (OnDamageDelegate.IsBound() == true) OnDamageDelegate.Execute(data, EAttackType::RANGE_DAMAGE);
		}
	}
}


void UCWeaponManagerComponent::SpawnWeapon(UCDA_WeaponData * requestWeapon)
{
	/** ||만약 다른 무기를든 상태에서 바로 바뀌는거면 이미 들고있던건 해제해준다..|| */
	if (currentWeapon != nullptr && currentWeapon->weaponName != requestWeapon->weaponName)
	{
		UnSpawnWeapon();
	}

	/** ||요청한 무기의 개수가 현재 동적할당된 메시컴포넌트 수 보다 많다면 더 동적할당 해온다.|| */
	if (requestWeapon == nullptr) return;
	while(usingWeaponStaticMeshes.Num() < requestWeapon->weaponData.Num())
	{///개수가 같아지는 순간 반복문 해제
		usingWeaponStaticMeshes.Emplace(NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass()));
		niagaraTempStaticMeshes.Emplace(NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass()));
		if (usingWeaponStaticMeshes.Last() == nullptr) return;
		usingWeaponStaticMeshes.Last()->RegisterComponent();//동적으로 만들때 사용한다.
	}
	

	/** ||이미 같은 무기를 들고 있었다면 스폰하지 않습니다.|| */
	if (currentWeapon != nullptr)
	{
		if (currentWeapon->weaponName == requestWeapon->weaponName) return;
	}


	/** ||요청한 무기의 메시를 세팅해주고 Material도 그대로기 때문에 변경하고 각 소켓위치에 부착해준다.|| 
	 */
	int i = 0;
	traceSocketLoc.Empty();
	for (FWeaponData & data : requestWeapon->weaponData)
	{///요청한 무기 개수만큼 실행
		if(data.weaponStaticmesh == nullptr)
		{
			DEBUG_WEAPON {  CLog::Log(L"weaponStaticmesh is null!!");}
			return;
		}
		
		usingWeaponStaticMeshes[i]->SetStaticMesh(data.weaponStaticmesh);
		int matIndex = 0;
		for (FStaticMaterial tempMat : data.weaponStaticmesh->StaticMaterials)
		{
			usingWeaponStaticMeshes[i]->SetMaterial(matIndex, tempMat.MaterialInterface);
			++matIndex;
		}
		usingWeaponStaticMeshes[i]->AttachToComponent(
			owner->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			data.toBeEquipSocketName
		);

		traceSocketLoc.Emplace(FLocArray(data.traceSocketCnt));
		++i;
	}

	//자연스러운 생성을 위해 머터리얼 값 조절 할 타임라인 실행
	currentWeapon = requestWeapon;
	CurveTimeline.PlayFromStart();
}


void UCWeaponManagerComponent::SpawnWeaponMaterial(float value)
{
	for (int i = 0; i < usingWeaponStaticMeshes.Num(); ++i)
	{
		if(usingWeaponStaticMeshes.IsValidIndex(i) == false) break;
		if (usingWeaponStaticMeshes[i]->GetStaticMesh() == false)break;

		usingWeaponStaticMeshes[i]->SetScalarParameterValueOnMaterials(FName("erode_alpha"), value);
	}
}


void UCWeaponManagerComponent::UnSpawnWeapon()
{
	//TODO : 맨손은 예외처리로 일단 이렇게 해놨는데 나중에 과연..?
	if (currentWeapon == nullptr) return;
	if (currentWeapon->weaponName != "Hand")
	{
		int i = 0;
		for (UStaticMeshComponent*& data : usingWeaponStaticMeshes)
		{
			if (data == nullptr) break;
			if (data->GetStaticMesh() != nullptr)
			{
				niagaraTempStaticMeshes[i]->SetStaticMesh(data->GetStaticMesh());
				niagaraTempStaticMeshes[i]->SetWorldTransform(data->GetComponentTransform());

				UNiagaraFunctionLibrary::SpawnSystemAttached(unSpawnEffect, niagaraTempStaticMeshes[i], TEXT("None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true, true, ENCPoolMethod::AutoRelease, true);

				//타임라인이 실행중이면 0으로 설정해도 다시 타임라인값으로 설정되기때문에 중단 해줘야함
				if (CurveTimeline.IsPlaying() == true)
				{
					CurveTimeline.Stop();
				}
				data->SetScalarParameterValueOnMaterials(FName("erode_alpha"), 0.0f);
			}
			++i;
		}
	}

	for (int i = 0; i < usingWeaponStaticMeshes.Num(); ++i)
	{
		if (usingWeaponStaticMeshes.IsValidIndex(i) == false) break;

		usingWeaponStaticMeshes[i]->SetStaticMesh(nullptr);
	}

	currentWeapon = nullptr;
}

void UCWeaponManagerComponent::EndUnSpawnEffect()
{

}

void UCWeaponManagerComponent::SetDoTrace_False()
{
	bDoTrace = false; 
	ResetHitsEmeny(); 
}


void UCWeaponManagerComponent::ResetHitsEmeny()
{
	hitsEnemy.Empty();
}



void UCWeaponManagerComponent::Debug()
{

	/*FString test = owner->GetName();
	test += " weaponNum : ";
	CLog::ScreenLog(usingWeaponStaticMeshes.Num(), -1.0f, FColor::Red, test);*/

	/*if (usingWeaponStaticMeshes.IsValidIndex(0) == true)
	{
		CLog::ScreenLog(usingWeaponStaticMeshes.Num(), -1.0f, FColor::Red, "usingWeaponNum : ");
		{
			for (int i = 0; i < usingWeaponStaticMeshes.Num(); ++i)
			{
				if (usingWeaponStaticMeshes.IsValidIndex(i) == false) break;
				FString test = FString::FromInt(i) + ". usingWeapon List : ";
				test += usingWeaponStaticMeshes[i]->GetStaticMesh()->GetName();
				CLog::ScreenLog(test, -1.0f, FColor::Blue);
				++i;
			}
		}
	}*/

	if (currentWeapon != nullptr)
	{
		FString test = "current weapon : ";
		test += currentWeapon->weaponName;
		CLog::ScreenLog(test, -1.0f, FColor::Red);
	}

	
}