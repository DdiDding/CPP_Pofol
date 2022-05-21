#include "Notify/CN_SetCanDoAttack_True.h"
#include "Actors/Character/CCharacter.h"
#include "Components/CSkillManagerComponent.h"

void UCN_SetCanDoAttack_True::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACCharacter * owner = Cast<ACCharacter> (MeshComp->GetOwner());
	if (owner == nullptr) return;

	UCSkillManagerComponent * smc = Cast<UCSkillManagerComponent>(owner->GetComponentByClass(UCSkillManagerComponent::StaticClass()));
	if (smc == nullptr) return;

	smc->SetCanDoAttack_true();
}