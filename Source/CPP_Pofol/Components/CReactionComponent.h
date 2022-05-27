#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CReactionComponent.generated.h"

class ACCharacter;
class UCGameInstance;
enum class EReactionType : uint8;
//DECLARE_DELEGATE(FHitted);
DECLARE_DELEGATE_OneParam(FHitted, EReactionType);

UENUM(BlueprintType)
enum class EHittedState : uint8
{
	NORMAL = 0, AIR , LAY_DOWN
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_POFOL_API UCReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCReactionComponent();

protected:
	virtual void BeginPlay() final;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
		UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void TakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	/**
	 *	���ݿ� �¾Ƽ� ������ �ɷ��� �� ���͸� �����ִ� �Լ� ShakeActor�� On,Off�� ����ϴ� ���
	 *	������ ������ ���°� ���߾�� �ϱ⶧���� GameInstance�� �������� �Լ����� ȣ��˴ϴ�.
	 */
	UFUNCTION()
	void EndShakeActor();

////////////////////////////////////////////////////////////////////////////////
public://Delegate

	FHitted OnHitted;

////////////////////////////////////////////////////////////////////////////////
private://member function

	//���� �ν��Ͻ����� �����
	UFUNCTION()
	void StartShakeActor();

	UFUNCTION()
	void DoingShakeActor();

	UFUNCTION()
	void KnockBackActor_Forward(float forwardAmount);

	UFUNCTION()
	void KnockBackActor_Upper(float upAmount);

	UFUNCTION()
	void SaveOwnerMaterial();

	UFUNCTION()
	void StartRimRight();

	UFUNCTION()
	void DoingRimRight(float value);

	UFUNCTION()
	void ReactionHandle(const EReactionType & reactionType, const FVector & knockBackAmount);

	/** ���߿��� Hitted�Ǿ��� �� ü���ð��� ���̱� ���� �߷� ���� ���� �����ϰ� 
	 *	�������� �߷°��� Ÿ�̸Ӹ� �����Ͽ� �ٽ� ���������ϴ�.
	 */
	UFUNCTION()
	void SetTimerForGravityInAirState();

	/* ������ �߷����� �ǵ����ϴ�.*/
	UFUNCTION()
	void SetOriginGravity();

////////////////////////////////////////////////////////////////////////////////
public://Get & Set Func


////////////////////////////////////////////////////////////////////////////////
private://member property

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Onwer Info", meta = (AllowPrivateAccess = true))
	ACCharacter * owner;

	//���� � ������ ������ �������ְ� ��� ������ �������� ���� ���¿��� ������ ���� ���� �Դϴ�.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Onwer Info", meta = (AllowPrivateAccess = true))
	EHittedState hittedState{ EHittedState::NORMAL };
	
	//�������� �� ���� ��ġ
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Damage Info", meta = (AllowPrivateAccess = true))
	FVector damagedActorLoc;

	//ShakeActor�� ������Ʈ �Ұ�����? - true�� Tick���� ������Ʈ�� �Ѵ�.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	bool bDoUpdateShake{ false };

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	float shakeAmount{ 6.5f };

	//����ũ�������� ���� Relative Location
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	FVector originRelativeLoc;

	//������Ʈ�� Ű������ �޽� 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	TArray<class UMaterialInstanceDynamic*> ownerMaterials;

	//Ÿ�Ӷ��ο� ���� �� Ŀ��, �ϴ� �������� ���� �մϴ�.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	UCurveFloat * CurveFloat;

	FTimeline CurveTimeline;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	float curveHalfLength;

	//�߷��� �����ϱ� ���� Ÿ�̸��� �ڵ�
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	FTimerHandle gravityHandle;

	//���߿��� �¾��� �� ü���ð��� ���̱� ���� ������ �߷°�
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	float upperGravity{ 0.9f };

	//�߷��� ���� �����ǰ� �ٽ� ���� �߷°����� �ǵ��ư��� ������ �ð�
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	float gravityTimerInRate = 0.3f;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "World Data", meta = (AllowPrivateAccess = true))
		UCGameInstance * gameInstance;

////////////////////////////////////////////////////////////////////////////////
public://Debug Property
	
};


/*UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);*/