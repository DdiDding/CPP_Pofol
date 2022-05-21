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
	 *	���⸦ ��ȯ �մϴ�. SkillManagerComponent(=SMC)���� ��������Ʈ ȣ��� ����˴ϴ�.
	 *	@inputWeapon - SMC���� ����Ǵ� ��ų�� �ش�Ǵ� ���Ⱑ �Ѿ�ɴϴ�.
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
	 *	UpdateTrace_PointDamageó�� �������� �˻��ϴ°� �ƴ� �������� �ѹ��� �������� �ִ°� 
	 *	@param addLoc	- ���� ���� �����̱� ������ Ʈ���̽��� owner�� ��ġ �߽��̱⿡ �ű⼭ �󸶳� �� ��ġ�� ���� �ٰ���
	 *	@param radius	- Ʈ���̽��� Sphere���·� ��� ������ �� ������
	 *	@param dotRange	- ���� ��ŭ���� üũ�Ұ��ΰ�? ���� ��
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateTrace_MeleeRangeDamage(FVector addLoc, float radius, float dotRange = 0.0f);

private:

	/** ����� �浹�˻縦 �Ͽ� �浹�ÿ� ��������Ʈ�� ������ SkillManagerComponent���� DoDamage�� ����˴ϴ�. */
	UFUNCTION()
	void UpdateTrace_PointDamage();


	/**
	 *	���� ���͸��� �Ķ������ 'erode' �� �����ϴ� �Լ�,
	 *	�Լ������� ���� ȣ���̾ƴ϶� Ÿ�Ӷ��ο� Bind �Ǿ� Ÿ�Ӷ����� ȣ��ɶ� �����,
	 *	��UFUNCTION�� �� �ٿ�����!
	 *	@param value - Ÿ�Ӷ����� Ŀ�� flaot�� ��
	 */
	UFUNCTION()
	void SpawnWeaponMaterial(float vaule);


	/**
	 *	UnSpawn����Ʈ�� ������ ������ �ִ� �޸𸮸� �����Ϸ��� NiagaraSystem�� ��������Ʈ(FOnSystemCompiled)�� ���� ȣ���Ѵ�.
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

	//�ߺ��� ���� �˻��Ϸ��� ���� �����ϴ� �迭
	UPROPERTY()
	TArray<AActor*> hitsEnemy;

	//Ʈ���̽��� �����ϴ°�?
	bool bDoTrace{ false };

	//������ Ʈ���̽� ������ġ�� ������ ������ �迭, �迭 �ǹ� : [���ⰳ��][�������� ���ϰ���]
	UPROPERTY()
	TArray<FLocArray> traceSocketLoc;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UCDA_WeaponData * currentWeapon;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<class UStaticMeshComponent*> usingWeaponStaticMeshes;

	//UnSpawn�ϱ� ���� �ӽ� SM������Ʈ
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<class UStaticMeshComponent*> niagaraTempStaticMeshes;

	//UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "SpawnWeapon", meta = (AllowPrivateAccess = true))
	FTimeline CurveTimeline;

	//Ÿ�Ӷ��ο� ���� �� Ŀ��, �ϴ� �������� ���� �մϴ�.
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
