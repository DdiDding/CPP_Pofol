#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CESkillType.generated.h"

//��� ������ ��������
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	POINT_DAMAGE = 0, RANGE_DAMAGE, GRAPPLER, MAX
};

//���� Ư���� ���׼��� �ʿ�����, �ƴϸ� DEFAULT
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
