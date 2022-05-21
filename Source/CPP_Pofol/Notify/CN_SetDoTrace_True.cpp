#include "Notify/CN_SetDoTrace_True.h"
#include "Actors/Character/CCharacter.h"
#include "Components/CWeaponManagerComponent.h"

void UCN_SetDoTrace_True::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACCharacter * owner = Cast<ACCharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	UCWeaponManagerComponent * wmc = Cast<UCWeaponManagerComponent>(owner->GetComponentByClass(UCWeaponManagerComponent::StaticClass()));
	if (wmc == nullptr) return;

	wmc->SetDoTrace_True();
}