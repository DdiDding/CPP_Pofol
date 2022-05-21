#include "Notify/NotifyState/SetBlur/CNS_SetBlurFromCurve.h"
#include "GameFramework/Character.h"
#include "PlayerProperty/CPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Util/CLog.h"

void UCNS_SetBlurFromCurve::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	owner = Cast<ACharacter>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	animInstance = MeshComp->GetAnimInstance();
	if (animInstance == nullptr) return;

	CameraManager = Cast<ACPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(MeshComp->GetWorld(), 0));
	if (CameraManager == nullptr) return;
}

void UCNS_SetBlurFromCurve::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (owner == nullptr || animInstance == nullptr || CameraManager == nullptr) return;

	CameraManager->SetBlurFromCurve(animInstance->GetCurveValue(FName("blur_value")));
}

void UCNS_SetBlurFromCurve::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (owner == nullptr || animInstance == nullptr || CameraManager == nullptr) return;

	CameraManager->SetBlurFromCurve(0.0f);
}
