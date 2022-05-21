#include "Notify/NotifyState/Time/CNS_SetGlobalTime.h"
#include "CGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Util/CLog.h"

void UCNS_SetGlobalTime::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (MeshComp == nullptr) return;

	gameInstance = Cast<UCGameInstance>(UGameplayStatics::GetGameInstance(MeshComp->GetWorld()));
	if (gameInstance == nullptr) return;

	animInstance = MeshComp->GetAnimInstance();
	originTimeRate = UGameplayStatics::GetGlobalTimeDilation(MeshComp->GetWorld());
}

void UCNS_SetGlobalTime::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (gameInstance == nullptr || animInstance == nullptr) return;

	//�ִ�Ŀ�꿡�� �⺻���� 0�̾�� �ϱ� ������ 0����  -1�� ����������ҰŴ�.
	//�׷��� 1�� �����༭  0 ~ 1�� �ǰԲ� �Ѱ���
	
	float timeDeilation = animInstance->GetCurveValue(FName("global_time")) + 1.0f;
	timeDeilation = UKismetMathLibrary::NormalizeToRange(timeDeilation, 0.0f, originTimeRate);

	timeDeilation = FMath::Clamp(timeDeilation, 0.0001f, 20.0f);
	gameInstance->SetGlobalTimeFromCurve(timeDeilation);
}

void UCNS_SetGlobalTime::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (gameInstance == nullptr || animInstance == nullptr) return;

	gameInstance->SetGlobalTimeFromCurve(originTimeRate);
}
