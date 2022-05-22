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
	 *	공격에 맞아서 경직에 걸렸을 때 액터를 흔들어주는 함수 ShakeActor의 On,Off를 담당하는 토글
	 *	경직이 끝나면 흔드는걸 멈추어야 하기때문에 GameInstance의 경직관련 함수에서 호출됩니다.
	 */
	UFUNCTION()
	void EndShakeActor();

////////////////////////////////////////////////////////////////////////////////
public://Delegate

	FHitted OnHitted;

////////////////////////////////////////////////////////////////////////////////
private://member function

	//게임 인스턴스에서 실행됨
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

	/** 공중에서 Hitted되었을 때 체공시간을 높이기 위한 중력 조절 값을 세팅하고 
	 *	원상태의 중력값을 타이머를 세팅하여 다시 돌려놓습니다.
	 */
	UFUNCTION()
	void SetTimerForGravityInAirState();

	/* 원래의 중력으로 되돌립니다.*/
	UFUNCTION()
	void SetOriginGravity();

////////////////////////////////////////////////////////////////////////////////
public://Get & Set Func


////////////////////////////////////////////////////////////////////////////////
private://member property

	UPROPERTY()
	ACCharacter * owner;

	UPROPERTY()
	UCGameInstance * gameInstance;
	
	//데미지를 준 적의 위치
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Damaget Info", meta = (AllowPrivateAccess = true))
	FVector damagedActorLoc;

	//ShakeActor를 업데이트 할것인지? - true면 Tick에서 업데이트를 한다.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	bool bDoUpdateShake{ false };

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	float shakeAmount{ 6.5f };

	//쉐이크흔들기전의 기존 Relative Location
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ShakeActor", meta = (AllowPrivateAccess = true))
	FVector originRelativeLoc;

	//림라이트를 키기위한 메시 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	TArray<class UMaterialInstanceDynamic*> ownerMaterials;

	//타임라인에 적용 될 커브, 일단 블프에서 세팅 합니다.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	UCurveFloat * CurveFloat;

	FTimeline CurveTimeline;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Rim Right", meta = (AllowPrivateAccess = true))
	float curveHalfLength;

	//중력을 조절하기 위한 타이머의 핸들
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	FTimerHandle gravityHandle;

	//공중에서 맞았을 때 체공시간을 높이기 위해 낮춰질 중력값
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	float upperGravity{ 0.9f };

	//중력이 낮게 조정되고 다시 원래 중력값으로 되돌아가기 까지의 시간
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	float gravityTimerInRate = 0.3f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
		float upperAmount{ 0.0f };

	//넉백 양
	float knockBackAmount;
////////////////////////////////////////////////////////////////////////////////
public://Debug Property
	
};


/*UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);*/