#pragma once

#include "CoreMinimal.h"
#include "DamageType/CDamageType.h"
#include "CDamageType_Range.generated.h"

/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCDamageType_Range : public UCDamageType
{
	GENERATED_BODY()
	
public:
	//�߽� ��ġ
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector centerLoc;

	//�ݰ�
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float radius;
};
