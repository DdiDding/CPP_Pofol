#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CNS_SetRotationAttacking.generated.h"

class UCSkillManagerComponent;

/**
 * 이 State동안 공격중에 캐릭터가 카메라를 따라 회전합니다.
 */
UCLASS()
class CPP_POFOL_API UCNS_SetRotationAttacking : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime);

	UCSkillManagerComponent * smc;
	ACharacter * owner;
	float lastYaw{ 0.0f };

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float interpSpeed { 15 };
};
