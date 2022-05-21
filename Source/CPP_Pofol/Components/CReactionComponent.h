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
	void KnockBackActor();

	UFUNCTION()
	void SaveOwnerMaterial();

	UFUNCTION()
	void StartRimRight();

	UFUNCTION()
	void DoingRimRight(float value);

	void ReactionHandle(EReactionType reactionType);

	void PushUp(FVector pushDir);

////////////////////////////////////////////////////////////////////////////////
public://Get & Set Func


////////////////////////////////////////////////////////////////////////////////
private://member property
	UPROPERTY()
	ACCharacter * owner;

	UPROPERTY()
	UCGameInstance * gameInstance;
	
	//ShakeActor�� ������Ʈ �Ұ�����? - true�� Tick���� ������Ʈ�� �Ѵ�.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	bool bDoUpdateShake{ false };

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	float shakeAmount{ 6.5f };

	//����ũ�������� ���� Relative Location
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	FVector originRelativeLoc;

	//�������� �� ���� ��ġ
	FVector damagedActorLoc;

	//�˹� ��
	float knockBackAmount;

	//������Ʈ�� Ű������ �޽� 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	TArray<class UMaterialInstanceDynamic*> ownerMaterials;

	//Ÿ�Ӷ��ο� ���� �� Ŀ��, �ϴ� �������� ���� �մϴ�.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	UCurveFloat * CurveFloat;

	FTimeline CurveTimeline;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	float curveHalfLength;
};


/*UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);*/