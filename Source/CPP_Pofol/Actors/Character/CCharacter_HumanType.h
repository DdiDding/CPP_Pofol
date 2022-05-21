#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/CCharacter.h"
#include "CCharacter_HumanType.generated.h"

/**
 * 
 */
UCLASS()
class CPP_POFOL_API ACCharacter_HumanType : public ACCharacter
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////
public://Virtual Func

	ACCharacter_HumanType();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual bool AddUpdateGroundedRotation() override;

	virtual UAnimMontage * GetGetUpAnimaMontage(bool isFaceUp) override;

	virtual void SetLocation() override;

////////////////////////////////////////////////////////////////////////////////
protected://member property

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ragdoll Value")
	UAnimMontage * GetUpAnim_FaceUp;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ragdoll Value")
	UAnimMontage * GetUpAnim_FaceDown;
};
