#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CParticleManagerComponent.generated.h"

class ACCharacter;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_POFOL_API UCParticleManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCParticleManagerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Trail_Start(TArray<FName> setSocketNames, UNiagaraSystem*& effect);
	void Trail_End();

public:
	const FName GetCurrentSocketName() { return currentSocketName; }

private:

	ACCharacter * owner;

	//�������� �ʰ� ȿ���� ���Ƴ���
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<UNiagaraComponent*> trailObj;
		
	//�ð��� ���߸� Notify�� Begin�� �ٽ� ���۵ȴ�.
	//=> �� ������, �׷��⿡ �ߺ� ó�� �������� ������ �����ߴ� �����̸��� ������ �ִ´�.
	FName currentSocketName{""};
};
