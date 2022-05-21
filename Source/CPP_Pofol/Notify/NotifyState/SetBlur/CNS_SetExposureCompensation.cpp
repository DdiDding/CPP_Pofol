#include "Notify/NotifyState/SetBlur/CNS_SetExposureCompensation.h"
#include "GameFramework/Character.h"
#include "PlayerProperty/CPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Util/CLog.h"

void UCNS_SetExposureCompensation::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	animInstance = MeshComp->GetAnimInstance();
	if (animInstance == nullptr) return;

	CameraManager = Cast<ACPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(MeshComp->GetWorld(), 0));
	if (CameraManager == nullptr) return;
}


void UCNS_SetExposureCompensation::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (owner == nullptr || animInstance == nullptr || CameraManager == nullptr) return;

	CameraManager->SetExposureCompensation(animInstance->GetCurveValue(FName("exposure_value")));
}


void UCNS_SetExposureCompensation::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (owner == nullptr || animInstance == nullptr || CameraManager == nullptr) return;

	CameraManager->SetExposureCompensation_Zero();
}

