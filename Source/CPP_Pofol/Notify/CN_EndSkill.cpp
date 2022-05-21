#include "Notify/CN_EndSkill.h"
#include "Actors/Character/CCharacter.h"
#include "Components/CSkillManagerComponent.h"
#include "Util/CLog.h"

void UCN_EndSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACCharacter * owner = Cast<ACCharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	UCSkillManagerComponent * smc = Cast<UCSkillManagerComponent>(owner->GetComponentByClass(UCSkillManagerComponent::StaticClass()));
	if (smc == nullptr) return;

	smc->EndSkill();
}

