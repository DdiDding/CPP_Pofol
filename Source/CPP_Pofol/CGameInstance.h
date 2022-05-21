// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Structs/Character/CCharacterStruct.h"
#include "CGameInstance.generated.h"

class CDA_WeaponData;

UCLASS()
class CPP_POFOL_API UCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCGameInstance();

	UFUNCTION()
	virtual void OnStart() final;

////////////////////////////////////////////////////////////////////////////////
public://Time Manage

	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	void RequestAdjustTime(float rate, float customTime, AActor * target);

	//notify·Î »ç¿ë
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	void SetGlobalTimeFromCurve(float curveValue);

private:
	UFUNCTION()
	void OriginallyTime(float customTime, AActor * target);

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "TimeManager", meta = (AllowPrivateAccess = true))
	TArray<FTimerHandle> timerHandle;

	int timerHandelPoolNum{ 32 };
	int currentPoolNum{ 0 };

	FTimerDelegate timerDele;
////////////////////////////////////////////////////////////////////////////////
public:

	/*UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<UCDA_WeaponData*> playerWeaponData;*/
	
	/*UPROPERTY(BlueprintReadWrite)
		int32 playerEquipIndex;*/

////////////////////////////////////////////////////////////////////////////////
};
