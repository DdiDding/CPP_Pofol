#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/CCharacterAnimInstance_HumanType.h"
#include "CPlayerAnimInstance2.generated.h"

class UCWallRunComponent;
/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCPlayerAnimInstance2 : public UCCharacterAnimInstance_HumanType
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////
public://virtual function

	virtual void NativeBeginPlay() final;
	virtual void NativeInitializeAnimation() final;
	virtual void NativeUpdateAnimation(float DeltaSeconds) final;


////////////////////////////////////////////////////////////////////////////////
protected://member function

	virtual void UpdateGround_Movement() final;

	virtual void UpdateGround_Idle()final;

	virtual void UpdateAir()final;

	virtual void UpdateSubState()final;

	void UpdateWallRunValue();

////////////////////////////////////////////////////////////////////////////////
private://member property

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Wall Run Value", meta = (AllowPrivateAccess = true))
	float wallRunRoll{ 0.0f };

	UCWallRunComponent * wrc;
};
