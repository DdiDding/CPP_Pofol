#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DataAssets/CDA_WeaponData.h"
#include "AWeaponEquipComponent.generated.h"


class UNiagaraSystem;
class UCurveFloat;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_POFOL_API UAWeaponEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAWeaponEquipComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


////////////////////////////////////////////////////////////////////////////////
//���� �����ϴ� �κ�
public:
	/**
	*WeaponData�� �ѱ�� �װɷ� �ٲ��ٰ� �̰���
	*����� WeaponData���� ����SM�̶� ������� ������ ����ִ�
	*@param index - ������ȣ���� �Է¹���
	*/
	UFUNCTION(BlueprintCallable)
		void ChangeWeapon(UCDA_WeaponData* weaponData);

public:
	//���Ⱑ ���� SM, �ΰ� �̻��� ���Ⱑ �����ɼ��ֱ⿡ �迭�� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UStaticMeshComponent*> WeaponStaticMeshes;

	UFUNCTION(BlueprintCallable)
		void UnSpawnWeapon();

////////////////////////////////////////////////////////////////////////////////
//Ÿ�Ӷ��� �κ�
public:

	FTimeline CurveTimeline;

	//Ŀ��� �������� ����
	UPROPERTY(EditAnywhere)
		UCurveFloat* CurveFloat;

	/**
	*���� ���͸��� �Ķ������ 'erode' �� �����ϴ� �Լ�, 
	*�Լ������� ���� ȣ���̾ƴ϶� Ÿ�Ӷ��ο� Bind �Ǿ� Ÿ�Ӷ����� ȣ��ɶ� �����, 
	*��UFUNCTION�� �� �ٿ�����!
	*@param value - Ÿ�Ӷ����� Ŀ�� flaot�� ��
	*/
	UFUNCTION()
		void SpawnWeaponMaterial(float vaule);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UNiagaraSystem* unSpawnEffect;

private:
	//WeaponStaticMeshes�� �迭���� �ʱ�ȭ int
	int weaponSlotMax;

};
