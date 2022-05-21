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

	//애님커브에서 기본값이 0이어야 하기 때문에 0에서  -1로 내려서사용할거다.
	//그래서 1을 더해줘서  0 ~ 1이 되게끔 한거임
	
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
