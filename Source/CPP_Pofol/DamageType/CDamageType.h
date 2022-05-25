#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Enums/CESkillType.h"
#include "CDamageType.generated.h"

class UNiagaraSystem;

/**
 * �� �����ϳ��ϳ��� ���� �����Դϴ�.
 */
UCLASS()
class CPP_POFOL_API UCDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UCDamageType() {};

	//�����ð�
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float stiffness;

	//�з����� ���� X = �������� �з����� ��, Z = ���� ���������(Upper Atk���� ���)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector knockBackPower;

	//���� ���� Ÿ������
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EAttackType attackType;

	//� ���׼� Ÿ��
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EReactionType reactionType;

	//���� �����鼭 ����� ��ƼŬ
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UNiagaraSystem*> useParticleToEnemy;

	//post preocess

	//ī�޶� ����ũ
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UCameraShakeBase> cameraShake;
	//camera anim
	
};
