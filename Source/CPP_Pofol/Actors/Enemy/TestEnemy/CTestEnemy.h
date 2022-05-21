#pragma once

#include "CoreMinimal.h"
#include "Actors/Enemy/CEnemy.h"
#include "CTestEnemy.generated.h"

/**
 * 
 */
UCLASS()
class CPP_POFOL_API ACTestEnemy : public ACEnemy
{
	GENERATED_BODY()
protected:
	ACTestEnemy();
	virtual void BeginPlay() override;
private:
	virtual void Tick(float DeltaTime) override;

};
