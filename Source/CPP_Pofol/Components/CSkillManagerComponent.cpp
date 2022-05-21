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
		{///�ش� Ű�� ��ų������ ã��
			if (StartSkill(temp.skill) == false)
			{
				DEBUG_SKILLMANAGER{ CLog::Log(L"StartSkill is fail..."); }
			}
		}
	}
}

//TODO : ���߿� ���⼭ EAttackType���� ���Ÿ��� ���� �����ؾ߰ڴµ�
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


	//��ų�� �ٸ��� �޺��ε��� �ʱ�ȭ!
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

	//���� ��ȯ
	if (OnSpawnWeaponDelegate.IsBound() == true) OnSpawnWeaponDelegate.Execute(currentSkill->spawnWeaponData);
	//�迭 �ʱ�ȭ
	if (OnResetHitArrayDelegate.IsBound() == true) OnResetHitArrayDelegate.Execute();
	//�ִ� ����
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

	//���߿� ��� Yaw���� �޾ƾ��ϴ°Ÿ� �� �̰� ������ �ٸ��� �����غ���
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

	/**	|| ������ ������ ������ ���� �˸��� ������ Ÿ���� �����ϰ� ������ �̺�Ʈ�� ���� �մϴ�. || */
	TSubclassOf<UDamageType> useDamageType;
	if (attackType == EAttackType::POINT_DAMAGE)
	{
		useDamageType = currentSkill->damageType[atkAnimIndex - 1];
	}
	if (attackType == EAttackType::RANGE_DAMAGE)
	{
		useDamageType = rangeDamageType;
	}


	//���⼭ �и�ó��??

	UGameplayStatics::ApplyPointDamage(
		hitInform.GetActor(),
		50.0f,
		hitInform.ImpactNormal,
		hitInform,
		nullptr,
		owner,
		useDamageType
	);


	/**	|| �����ϴ� ������ ȿ���� ���� ó�� �Դϴ�. || 
	 *	1. �������ý���, ���� 0�̸� ���ص� ó�� ���� �ʽ��ϴ�. (�´� ������ ������ HitReaction���� ���)
	 *	2. ī�޶� ����ũ
	 */
	UCDamageType * damageInst = Cast<UCDamageType>(useDamageType->GetDefaultObject(true));
	
	//!!������ �κ��Դϴ�. �����κ��� �ƴմϴ�!
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
