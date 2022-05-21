#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CNS_SetGlobalTime.generated.h"

class UCGameInstance;
class UAnimInstance;
/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCNS_SetGlobalTime : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);

	UCGameInstance * gameInstance;
	UAnimInstance * animInstance;

	//조정 전의 시간
	float originTimeRate;
};
