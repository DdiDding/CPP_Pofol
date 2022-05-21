#include "CNS_SetRotationAttacking.h"
#include "Components/CSkillManagerComponent.h"

void UCNS_SetRotationAttacking::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (MeshComp == nullptr) return;

	owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;
	lastYaw = owner->GetControlRotation().Yaw;

	smc = Cast<UCSkillManagerComponent>(owner->GetComponentByClass(UCSkillManagerComponent::StaticClass()));
	if (smc == nullptr) return;
}

void UCNS_SetRotationAttacking::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (owner == nullptr || smc == nullptr) return;

	//º¸°£
	lastYaw = FMath::FInterpTo(lastYaw, owner->GetControlRotation().Yaw, FrameDeltaTime, interpSpeed);
	smc->SetAttackYaw(lastYaw);
}