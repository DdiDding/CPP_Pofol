#include "Notify/CN_DoDamage_MeleeRange.h"
#include "Actors/Character/CCharacter.h"
#include "DamageType/CDamageType_Range.h"
#include "Components/CWeaponManagerComponent.h"
#include "Components/CSkillManagerComponent.h"


void UCN_DoDamage_MeleeRange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACCharacter * owner = Cast<ACCharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	UCWeaponManagerComponent * wmc = Cast<UCWeaponManagerComponent>(owner->GetComponentByClass(UCWeaponManagerComponent::StaticClass()));
	if (wmc == nullptr) return;

	UCSkillManagerComponent * smc = Cast<UCSkillManagerComponent>(owner->GetComponentByClass(UCSkillManagerComponent::StaticClass()));
	if (smc == nullptr) return;

	UCDamageType_Range * damageTypeObj = Cast<UCDamageType_Range>(smc->GetRangeDamageType()->GetDefaultObject(true));
	if (IsValid(damageTypeObj) == false) return;

	wmc->UpdateTrace_MeleeRangeDamage(damageTypeObj->centerLoc, damageTypeObj->radius, dotRange);
}

