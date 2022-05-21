#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CCharacterStruct.generated.h"

UENUM(BlueprintType)
enum class EMainState : uint8
{
	GROUND = 0, AIR, MAX
};

UENUM(BlueprintType)
enum class ESubState : uint8
{
	NONE = 0, WALLRUN, ATTACK, HITTED, LAY_DOWN, GRAPPLING, GET_UP, MAX
};

UENUM(BlueprintType)
enum class EMoveMode : uint8
{
	WALKING = 0, RUNNING, MAX
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	NONE = 0, VELOCITY, CONTROLLER, MAX
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NONE = 0, GREATSWORD, TWINDAGGER, MAX
};

UCLASS()
class CPP_POFOL_API UCCharacterStruct : public UObject
{
	GENERATED_BODY()

public:
	UCCharacterStruct();

////////////////////////////////////////////////////////////////////////////////
public://EMainState

	UFUNCTION(BlueprintPure)
		const EMainState GetMainState();

	UFUNCTION(BlueprintCallable)
		void SetMainState_Ground();
	UFUNCTION(BlueprintCallable)
		void SetMainState_Air();


////////////////////////////////////////////////////////////////////////////////
public://ESubState

	UFUNCTION(BlueprintPure)
		const ESubState GetSubState();

	UFUNCTION(BlueprintCallable)
		void SetSubState_None();
	UFUNCTION(BlueprintCallable)
		void SetSubState_Wallrun();
	UFUNCTION(BlueprintCallable)
		void SetSubState_Attack();
	UFUNCTION(BlueprintCallable)
		void SetSubState_Hitted();
	UFUNCTION(BlueprintCallable)
		void SetSubState_LayDown();
	UFUNCTION(BlueprintCallable)
		void SetSubState_Grappling();
	UFUNCTION(BlueprintCallable)
		void SetSubState_GetUp();

////////////////////////////////////////////////////////////////////////////////
public://Weapon State

	UFUNCTION(BlueprintPure)
		const EWeaponState GetWeaponState();

	UFUNCTION(BlueprintCallable)
		void SetDefault();
	UFUNCTION(BlueprintCallable)
		void SetGreatSword();
	UFUNCTION(BlueprintCallable)
		void SetTwinDagger();

////////////////////////////////////////////////////////////////////////////////
public://ERotationMode

	UFUNCTION(BlueprintCallable)
		const ERotationMode GetRotationMode();

	UFUNCTION(BlueprintCallable)
		void SetRotationMode_None();
	UFUNCTION(BlueprintCallable)
		void SetRotationMode_Controller();
	UFUNCTION(BlueprintCallable)
		void SetRotationMode_Velocity();

////////////////////////////////////////////////////////////////////////////////
public://EMove Mode

	UFUNCTION(BlueprintPure)
		const EMoveMode GetMoveMode();

	UFUNCTION(BlueprintCallable)
		void SetMoveMode_Walking();
	UFUNCTION(BlueprintCallable)
		void SetMoveMode_Running();


////////////////////////////////////////////////////////////////////////////////
private:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "EnumState", meta = (AllowPrivateAccess = true))
	EMainState eMainState;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "EnumState", meta = (AllowPrivateAccess = true))
	ESubState eSubState;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "EnumState", meta = (AllowPrivateAccess = true))
	EMoveMode eMoveMode;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "EnumState", meta = (AllowPrivateAccess = true))
	ERotationMode eRotationMode;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "EnumState", meta = (AllowPrivateAccess = true))
	EWeaponState eWeaponState;
};
