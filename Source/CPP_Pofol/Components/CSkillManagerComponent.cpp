#include "CSkillManagerComponent.h"
#include "CWeaponManagerComponent.h"
#include "DataAssets/CDA_Skill.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CGameInstance.h"
#include "Actors/Character/CCharacter.h"
#include "DamageType/CDamageType.h"
#include "DamageType/CDamageType_Range.h"
#include "Util/CLog.h"


UCSkillManagerComponent::UCSkillManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCSkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	owner = Cast<ACCharacter>(GetOwner());
	if(owner != nullptr)
	{
		UCWeaponManagerComponent * wmc = Cast<UCWeaponManagerComponent>(owner->GetComponentByClass(UCWeaponManagerComponent::StaticClass()));
		if (wmc != nullptr)
		{
			wmc->OnDamageDelegate.BindUFunction(this, FName("DoDamage"));
		}
	}

	gameInstance = Cast<UCGameInstance>(GetWorld()->GetGameInstance());
	check(gameInstance);
}


void UCSkillManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UCSkillManagerComponent::LaunchSkill(FName key)
{
	for (FSkillSlot & temp : skillSlots)
	{
		if (temp.slotName == key)
		{///해당 키의 스킬슬롯을 찾음
			if (StartSkill(temp.skill) == false)
			{
				DEBUG_SKILLMANAGER{ CLog::Log(L"StartSkill is fail..."); }
			}
		}
	}
}

//TODO : 나중에 여기서 EAttackType별로 원거리는 따로 세팅해야겠는디
bool UCSkillManagerComponent::StartSkill(UCDA_Skill * skill)
{

	if (skill == nullptr)
	{
		DEBUG_SKILLMANAGER{ CLog::Log(L"Came in SkillData is null.."); }
		return false;
	}
	if (bCanDoAttack == false)
	{ 
		return false; 
	}


	//스킬이 다르면 콤보인덱스 초기화!
	if (currentSkill != nullptr && skill->skillName != currentSkill->skillName)
	{
		atkAnimIndex = 0;
	}
	
	currentSkill = skill;
	if(currentSkill->attackAnims.Num() <= atkAnimIndex)
	{
		DEBUG_SKILLMANAGER{ CLog::Log(L"Finding over range in SkillMotage..."); }
		EndSkill();
		return false;
	}

	//무기 소환
	if (OnSpawnWeaponDelegate.IsBound() == true) OnSpawnWeaponDelegate.Execute(currentSkill->spawnWeaponData);
	//배열 초기화
	if (OnResetHitArrayDelegate.IsBound() == true) OnResetHitArrayDelegate.Execute();
	//애님 실행
	owner->PlayAnimMontage(currentSkill->attackAnims[atkAnimIndex]);
	
	if (owner->GetCharacterStruct() == nullptr)
	{
		DEBUG_SKILLMANAGER{ CLog::Log(L"Don't Find owner Character Struct in SkillManager..."); }
		return false;
	}
	
	StartSkill_Internal();
	return true;
}


void UCSkillManagerComponent::StartSkill_Internal()
{
	owner->GetCharacterStruct()->SetSubState_Attack();
	bCanDoAttack = false;
	++atkAnimIndex;

	//나중에 계속 Yaw값을 받아야하는거면 또 이걸 가지고 다르게 생각해보기
	attackYaw = owner->GetControlRotation().Yaw;
}


void UCSkillManagerComponent::EndSkill()
{
	atkAnimIndex = 0;
	bCanDoAttack = true;
	currentSkill = nullptr;

	if (owner->GetCharacterStruct() == nullptr)
	{
		DEBUG_SKILLMANAGER{ CLog::Log(L"Don't Find owner Character Struct in SkillManager..."); }
		return;
	}
	owner->GetCharacterStruct()->SetSubState_None();
	if(OnEndSkill.IsBound() == true) OnEndSkill.Execute();
}


void UCSkillManagerComponent::DoDamage(FHitResult& hitInform, EAttackType attackType)
{
	if (currentSkill == nullptr) return;
	if (currentSkill->damageType.Num() <= atkAnimIndex - 1) return;

	/**	|| 공격이 들어오는 유형에 따라 알맞은 데미지 타입을 결정하고 데미지 이벤트를 실행 합니다. || */
	TSubclassOf<UDamageType> useDamageType;
	if (attackType == EAttackType::POINT_DAMAGE)
	{
		useDamageType = currentSkill->damageType[atkAnimIndex - 1];
	}
	if (attackType == EAttackType::RANGE_DAMAGE)
	{
		useDamageType = rangeDamageType;
	}


	//여기서 패링처리??

	UGameplayStatics::ApplyPointDamage(
		hitInform.GetActor(),
		50.0f,
		hitInform.ImpactNormal,
		hitInform,
		nullptr,
		owner,
		useDamageType
	);


	/**	|| 공격하는 액터의 효과에 대한 처리 입니다. || 
	 *	1. 역경직시스템, 값이 0이면 안해도 처리 하지 않습니다. (맞는 액터의 경직은 HitReaction에서 담당)
	 *	2. 카메라 셰이크
	 */
	UCDamageType * damageInst = Cast<UCDamageType>(useDamageType->GetDefaultObject(true));
	
	//!!역경직 부분입니다. 경직부분이 아닙니다!
	if (FMath::IsNearlyZero(damageInst->stiffness) == false)
	{
		gameInstance->RequestAdjustTime(damageInst->stiffness, 0.0f, owner);
	}

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(
		damageInst->cameraShake);

	return;
}

//if (damageInst->attackType == EAttackType::RADIAL_DAMAGE)
//{
//	UGameplayStatics::ApplyRadialDamage(
//		GetWorld(), 50.0f, hitInform.ImpactPoint, 1000.0f, currentSkill->damageType[atkAnimIndex - 1], TArray<AActor*>(), owner
//	);
//	/*UGameplayStatics::ApplyDamage(
//		hitInform.GetActor(),
//		50.0f, nullptr, owner, currentSkill->damageType[atkAnimIndex - 1]);*/
//}
