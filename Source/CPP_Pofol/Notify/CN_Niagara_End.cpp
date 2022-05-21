#include "Notify/CN_Niagara_End.h"
#include "Components/CParticleManagerComponent.h"
#include "Util/CLog.h"

void UCN_Niagara_End::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor * owner = Cast<AActor>(MeshComp->GetOwner());
	if (owner == nullptr) return;

	UCParticleManagerComponent * pmc = Cast<UCParticleManagerComponent>(owner->GetComponentByClass(UCParticleManagerComponent::StaticClass()));
	if (pmc == nullptr) return;

	//여기서 소켄이름 검사해서 같으면 다시재생, 다르면 생성? 
	pmc->Trail_End();
}
