#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CN_SetRangeDamageType.generated.h"

class UCDamageType_Range;
/**
 * Skill Manager component에서 CurrentRadialDamageType를 세팅해주는 목적이있다.
 */
UCLASS()
class CPP_POFOL_API UCN_SetRangeDamageType : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)final;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UCDamageType_Range> damageType;
};
