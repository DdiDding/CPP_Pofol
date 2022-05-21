#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CN_SetRangeDamageType.generated.h"

class UCDamageType_Range;
/**
 * Skill Manager component���� CurrentRadialDamageType�� �������ִ� �������ִ�.
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
