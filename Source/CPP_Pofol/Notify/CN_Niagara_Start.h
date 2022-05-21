// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CN_Niagara_Start.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCN_Niagara_Start : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)final;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UNiagaraSystem * effect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FName> setSocketNames;
};
