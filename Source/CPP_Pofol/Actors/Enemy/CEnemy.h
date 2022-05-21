#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/CCharacter_HumanType.h"
#include "CEnemy.generated.h"

UCLASS()
class CPP_POFOL_API ACEnemy : public ACCharacter_HumanType
{
	GENERATED_BODY()
protected:
	ACEnemy();
	virtual void BeginPlay() override;
protected:
	virtual void Tick(float DeltaTime) override;
};
