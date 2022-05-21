// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CN_SetCanDoAttack_True.generated.h"

/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCN_SetCanDoAttack_True : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)final;
};
