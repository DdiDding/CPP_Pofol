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
	//중심 위치
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector centerLoc;

	//반경
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float radius;
};
