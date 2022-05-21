#include "Notify/CN_SetDoTrace_False.h"
#include "Actors/Character/CCharacter.h"
#include "Components/CWeaponManagerComponent.h"

void UCN_SetDoTrace_False::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACCharacter * owner = Cast<ACCharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	UCWeaponManagerComponent * wmc = Cast<UCWeaponManagerComponent>(owner->GetComponentByClass(UCWeaponManagerComponent::StaticClass()));
	if (wmc == nullptr) return;

	wmc->SetDoTrace_False();
}