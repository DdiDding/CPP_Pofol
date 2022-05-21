#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CESkillType.generated.h"

//어떤한 종류의 공격인지
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	POINT_DAMAGE = 0, RANGE_DAMAGE, GRAPPLER, MAX
};

//적이 특수한 리액션이 필요한지, 아니면 DEFAULT
UENUM(BlueprintType)
enum class EReactionType : uint8
{
	NORMAL = 0, SMASH_UPPER, SMASH_DOWN, STRONG, MAX
};

UCLASS()
class CPP_POFOL_API UCESkillType : public UObject
{
	GENERATED_BODY()
	
};
