#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums/CESkillType.h"
#include "CDA_Skill.generated.h"

class UNiagaraSystem;
class UCDA_WeaponData;
class UCDamageType;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCDA_Skill : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName skillName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UNiagaraSystem*> useParticleToMe;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UAnimMontage*> attackAnims;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray <TSubclassOf<UCDamageType>> damageType;
	
	//weapon spawn data
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCDA_WeaponData * spawnWeaponData;
};
