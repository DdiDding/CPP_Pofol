#include "Notify/CN_SetRangeDamageType.h"
#include "DamageType/CDamageType_Range.h"
#include "Components/CSkillManagerComponent.h"


void UCN_SetRangeDamageType::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor * owner = MeshComp->GetOwner();
	if (owner == nullptr) return;

	UCSkillManagerComponent * smc = Cast<UCSkillManagerComponent>(owner->GetComponentByClass(UCSkillManagerComponent::StaticClass()));
	if (smc == nullptr) return;

	smc->SetRangeDamageType(damageType);
}

