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

	//지우지는 않고 효과만 갈아끼자
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<UNiagaraComponent*> trailObj;
		
	//시간이 멈추면 Notify의 Begin이 다시 시작된다.
	//=> 재 생성됨, 그렇기에 중복 처리 막기위해 이전에 생성했던 소켓이름을 가지고 있는다.
	FName currentSocketName{""};
};
