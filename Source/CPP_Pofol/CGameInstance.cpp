#include "CGameInstance.h"
#include "DataAssets/CDA_WeaponData.h"
#include "Components/CReactionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Util/CLog.h"

UCGameInstance::UCGameInstance()
{
}

void UCGameInstance::OnStart()
{
	timerHandle.Init(FTimerHandle(), timerHandelPoolNum);
}

void UCGameInstance::RequestAdjustTime(float rate, float customTime, AActor * target)
{
	if (target == nullptr) return; 

	target->CustomTimeDilation = 0.0f;

	timerDele.BindUFunction(this, FName("OriginallyTime"), customTime, target);
	TimerManager->SetTimer(timerHandle[currentPoolNum], timerDele, rate, false);

	++currentPoolNum;
	if (currentPoolNum == timerHandelPoolNum)
	{
		currentPoolNum = 0;
	}
}

//TODO : ���߿� ���� �ӵ� �����ϱ� ex)���ο� �����µ� ������ 1.0�ٽ� �ɸ�
void UCGameInstance::OriginallyTime(float customTime, AActor * target)
{
	if (target == nullptr) return;
	target->CustomTimeDilation = 1.0f;
	Cast<UCReactionComponent>(target->GetComponentByClass(UCReactionComponent::StaticClass()))->EndShakeActor();
}

void UCGameInstance::SetGlobalTimeFromCurve(float curveValue)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), curveValue);
}

//
//UCDA_WeaponData* UCGameInstance::GetEquipWeaponData()
//{
//	//��ȿ�ϸ�
//	if (playerWeaponData.IsValidIndex(playerEquipIndex) == true) 
//	{ return playerWeaponData[playerEquipIndex]; }
//	//��ȿ���� ������
//	else
//	{ return nullptr; }
//}
//
//bool UCGameInstance::IsEquipWeaponSlot(int slotIndex)
//{
//	return slotIndex == playerEquipIndex ? true : false;
//}
//
