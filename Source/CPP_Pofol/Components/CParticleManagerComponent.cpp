#include "Components/CParticleManagerComponent.h"
#include "Actors/Character/CCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Util/CLog.h"

UCParticleManagerComponent::UCParticleManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCParticleManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	owner = Cast<ACCharacter>(GetOwner());
	check(owner);

}


void UCParticleManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UCParticleManagerComponent::Trail_Start(TArray<FName> setSocketNames, UNiagaraSystem*& effect)
{

	while (trailObj.Num() < setSocketNames.Num())
	{///들어오는 무기의 개수가 더 많으면 그만큼 할당해준다.
		trailObj.Emplace(NewObject<UNiagaraComponent>(this));
		if (trailObj.Last() == nullptr) return;
		trailObj.Last()->RegisterComponent();
	}


	int i = 0;
	for (UNiagaraComponent*& data : trailObj)
	{
		if (trailObj.IsValidIndex(i) == false) break;

		/*data->SetAsset(effect);
		data->AutoAttachSocketName = setSocketNames[i];*/

		if (data->GetAsset() != nullptr)
		{
			
		}

		data = UNiagaraFunctionLibrary::SpawnSystemAttached(effect, owner->GetMesh(), setSocketNames[i], FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true, true, ENCPoolMethod::AutoRelease, true);
		

		++i;
	}

	currentSocketName = setSocketNames[0];
}
void UCParticleManagerComponent::Trail_End()
{
	int i = 0;
	for (UNiagaraComponent*& data : trailObj)
	{
		data->Deactivate();

		++i;
	}
	/*UNiagaraFunctionLibrary::fini
	trailEffect[1]->*/
}