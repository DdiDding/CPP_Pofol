#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CDA_WeaponData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

public:
	//������ ����ƽ�޽�
	UPROPERTY(EditAnywhere)
	UStaticMesh * weaponStaticmesh;

	//���Ⱑ ���� �����̸�
	UPROPERTY(EditAnywhere)
	FName toBeEquipSocketName;

	//if weapon has socket from 0 ~ 4, put in data '5'
	UPROPERTY(EditAnywhere)
	int32 traceSocketCnt;
};

UCLASS()
class CPP_POFOL_API UCDA_WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//������ �̸�
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString weaponName;

	//������ ������
	//���� ���Ⱑ �ΰ�(twin daggers)�� �迭�� ũ�⵵ 2���� �ȴ�
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FWeaponData> weaponData;
};

