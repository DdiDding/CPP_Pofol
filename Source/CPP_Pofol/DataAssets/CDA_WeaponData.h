#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CDA_WeaponData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

public:
	//무기의 스태틱메시
	UPROPERTY(EditAnywhere)
	UStaticMesh * weaponStaticmesh;

	//무기가 붙을 소켓이름
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

	//무기의 이름
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString weaponName;

	//무기의 데이터
	//만약 무기가 두개(twin daggers)면 배열의 크기도 2개가 된다
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FWeaponData> weaponData;
};

