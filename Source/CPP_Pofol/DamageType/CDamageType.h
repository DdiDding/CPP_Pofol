#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Enums/CESkillType.h"
#include "CDamageType.generated.h"

class UNiagaraSystem;

/**
 * 각 공격하나하나의 공격 정보입니다.
 */
UCLASS()
class CPP_POFOL_API UCDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UCDamageType() {};

	//경직시간
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float stiffness;

	//밀려나는 정도 X = 전방으로 밀려나는 힘, Z = 위로 띄워지는힘(Upper Atk에만 사용)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector knockBackPower;

	//무슨 공격 타입인지
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EAttackType attackType;

	//어떤 리액션 타입
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EReactionType reactionType;

	//적이 맞으면서 사용할 파티클
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UNiagaraSystem*> useParticleToEnemy;

	//post preocess

	//카메라 셰이크
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UCameraShakeBase> cameraShake;
	//camera anim
	
};
