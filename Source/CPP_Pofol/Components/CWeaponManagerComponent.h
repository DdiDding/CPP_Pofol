#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CWeaponManagerComponent.generated.h"

#define DEBUG_WEAPON if(isDebugMode == true)

class ACCharacter;
class UCDA_WeaponData;
class UNiagaraSystem;
enum class EAttackType : uint8;

DECLARE_DELEGATE_TwoParams(FDamage, FHitResult&, EAttackType);

USTRUCT(Atomic, BlueprintType)
struct FLocArray
{
	GENERATED_BODY()

	FLocArray() {};
	FLocArray(int socketNum)
	{
		locArray.Init(FVector::ZeroVector, socketNum);
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> locArray;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CPP_POFOL_API UCWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()
////////////////////////////////////////////////////////////////////////////////
public://Default Func
	UCWeaponManagerComponent();
	~UCWeaponManagerComponent();

protected:
	virtual void BeginPlay() override;

private:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
	 *	무기를 소환 합니다. SkillManagerComponent(=SMC)에서 델리게이트 호출시 실행됩니다.
	 *	@inputWeapon - SMC에서 실행되는 스킬에 해당되는 무기가 넘어옵니다.
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnWeapon(UCDA_WeaponData * requestWeapon);

	UFUNCTION(BlueprintCallable)
	void UnSpawnWeapon();

	UFUNCTION(BlueprintCallable)
	void ResetHitsEmeny();

	/*void OnCollision();
	void OffCollision();*/

////////////////////////////////////////////////////////////////////////////////
public://Delegate

	FDamage OnDamageDelegate;


////////////////////////////////////////////////////////////////////////////////
public://member Function

	/**
	 *	UpdateTrace_PointDamage처럼 구간별로 검사하는게 아닌 광범위로 한번에 데미지를 주는것 
	 *	@param addLoc	- 근접 광역 공격이기 때문에 트레이스가 owner의 위치 중심이기에 거기서 얼마나 더 위치를 더해 줄건지
	 *	@param radius	- 트레이스를 Sphere형태로 쏘기 때문에 그 반지름
	 *	@param dotRange	- 전방 얼만큼까지 체크할것인가? 내적 값
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateTrace_MeleeRangeDamage(FVector addLoc, float radius, float dotRange = 0.0f);

private:

	/** 무기와 충돌검사를 하여 충돌시에 델리게이트를 실행해 SkillManagerComponent에서 DoDamage가 실행됩니다. */
	UFUNCTION()
	void UpdateTrace_PointDamage();


	/**
	 *	무기 머터리얼 파라미터중 'erode' 를 변경하는 함수,
	 *	함수내에서 직접 호출이아니라 타임라인에 Bind 되어 타임라인이 호출될때 실행됨,
	 *	★UFUNCTION을 꼭 붙여야함!
	 *	@param value - 타임라인의 커브 flaot형 값
	 */
	UFUNCTION()
	void SpawnWeaponMaterial(float vaule);


	/**
	 *	UnSpawn이펙트가 끝나면 가지고 있던 메모리를 해제하려고 NiagaraSystem의 델리게이트(FOnSystemCompiled)에 의해 호출한다.
	 */
	UFUNCTION()
	void EndUnSpawnEffect();


	void Debug();

////////////////////////////////////////////////////////////////////////////////
public://Set & Get Func

	void SetDoTrace_True() { bDoTrace = true; }
	void SetDoTrace_False();

////////////////////////////////////////////////////////////////////////////////
private:// member property
	UPROPERTY()
	ACCharacter * owner;

	//중복된 적을 검사하려고 액터 저장하는 배열
	UPROPERTY()
	TArray<AActor*> hitsEnemy;

	//트레이스를 실행하는가?
	bool bDoTrace{ false };

	//무기의 트레이스 소켓위치를 저장할 이차원 배열, 배열 의미 : [무기개수][각무기의 소켓개수]
	UPROPERTY()
	TArray<FLocArray> traceSocketLoc;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UCDA_WeaponData * currentWeapon;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<class UStaticMeshComponent*> usingWeaponStaticMeshes;

	//UnSpawn하기 위한 임시 SM컴포넌트
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<class UStaticMeshComponent*> niagaraTempStaticMeshes;

	//UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "SpawnWeapon", meta = (AllowPrivateAccess = true))
	FTimeline CurveTimeline;

	//타임라인에 적용 될 커브, 일단 블프에서 세팅 합니다.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SpawnWeapon", meta = (AllowPrivateAccess = true))
	UCurveFloat* CurveFloat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug", meta = (AllowPrivateAccess = true))
	bool isDebugMode{ true };


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UNiagaraSystem* unSpawnEffect;

	//arrow debug value
	float arrowLength{ 400.0f };
	float arrowSize{ 1000.0f };
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = true))
	float arrowThick{ 1.0f };
};
