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
//무기 세팅하는 부분
public:
	/**
	*WeaponData를 넘기면 그걸로 바꿔줄게 이거임
	*참고로 WeaponData에는 무기SM이랑 몇개들어가는지 정보가 들어있다
	*@param index - 장착번호슬롯 입력받음
	*/
	UFUNCTION(BlueprintCallable)
		void ChangeWeapon(UCDA_WeaponData* weaponData);

public:
	//무기가 들어간는 SM, 두개 이상의 무기가 장착될수있기에 배열로 생성
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UStaticMeshComponent*> WeaponStaticMeshes;

	UFUNCTION(BlueprintCallable)
		void UnSpawnWeapon();

////////////////////////////////////////////////////////////////////////////////
//타임라인 부분
public:

	FTimeline CurveTimeline;

	//커브는 블프에서 적용
	UPROPERTY(EditAnywhere)
		UCurveFloat* CurveFloat;

	/**
	*무기 머터리얼 파라미터중 'erode' 를 변경하는 함수, 
	*함수내에서 직접 호출이아니라 타임라인에 Bind 되어 타임라인이 호출될때 실행됨, 
	*★UFUNCTION을 꼭 붙여야함!
	*@param value - 타임라인의 커브 flaot형 값
	*/
	UFUNCTION()
		void SpawnWeaponMaterial(float vaule);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UNiagaraSystem* unSpawnEffect;

private:
	//WeaponStaticMeshes의 배열개수 초기화 int
	int weaponSlotMax;

};
