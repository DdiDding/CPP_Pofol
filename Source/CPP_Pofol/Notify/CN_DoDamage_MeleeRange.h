#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CN_DoDamage_MeleeRange.generated.h"

/**
 * ���� ���� ������ �����մϴ�.
 */
UCLASS()
class CPP_POFOL_API UCN_DoDamage_MeleeRange : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)final;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float dotRange{ 0.0f };
};
