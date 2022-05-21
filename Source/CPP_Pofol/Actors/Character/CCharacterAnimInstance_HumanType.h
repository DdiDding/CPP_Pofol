#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/CCharacterAnimInstance.h"
#include "CCharacterAnimInstance_HumanType.generated.h"

/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCCharacterAnimInstance_HumanType : public UCCharacterAnimInstance
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////
public://virtual function

	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

////////////////////////////////////////////////////////////////////////////////
protected://member function

	virtual void UpdateGround_Movement() override;
	virtual void UpdateGround_Idle()override;
	virtual void UpdateAir()override;
	virtual void UpdateSubState()override;
	virtual void UpdateFootIK() final;



////////////////////////////////////////////////////////////////////////////////
protected://member property


};
