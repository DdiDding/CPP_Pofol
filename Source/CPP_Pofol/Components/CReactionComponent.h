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
	UFUNCTION(Category = "Shake Actor")
	void StartShakeActor();

	UFUNCTION(Category = "Shake Actor")
	void DoingShakeActor();


	UFUNCTION(Category = "Knockback")
	void KnockBackActor_Forward(float forwardAmount);

	UFUNCTION(Category = "Knockback")
	void KnockBackActor_Upper(float upAmount);

	/** 공중에서 Hitted되었을 때 체공시간을 높이기 위한 중력 조절 값을 세팅하고
	 *	원상태의 중력값을 타이머를 세팅하여 다시 돌려놓습니다.
	 */
	UFUNCTION(Category = "Knockback")
		void SetTimerForGravityInAirState();

	/* 원래의 중력으로 되돌립니다.*/
	UFUNCTION(Category = "Knockback")
		void SetOriginGravity();


	UFUNCTION(Category = "Rim Right")
	void StartRimRight();

	UFUNCTION(Category = "Rim Right")
	void DoingRimRight(float value);


	//TODO:여기서 방어 성공했는지 확인하면 괜찮을듯
	/* Reaction유형에 따른 처리를 하고 공격 성공여부를 반환 */
	UFUNCTION(Category = "Hitted Logic")
	bool ReactionHandle(const EReactionType & reactionType, const FVector & knockBackAmount);

	/* 하단공격이 맞을수 있는지, True면 가능 false면 불가능*/
	UFUNCTION(Category = "Hitted Logic")
	bool CheckCanHittedBottom();

////////////////////////////////////////////////////////////////////////////////
public://Get & Set Func


////////////////////////////////////////////////////////////////////////////////
private://member property

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Onwer Info", meta = (AllowPrivateAccess = true))
	ACCharacter * owner;
	
	//현재 어떤 판정의 공격을 받을수있고 어떻게 판정이 들어가는지에 대한 상태에의 구분을 위한 변수 입니다.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Onwer Info", meta = (AllowPrivateAccess = true))
	EHittedState hittedState{ EHittedState::NORMAL };
	
	//데미지를 준 액터
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Damage Info", meta = (AllowPrivateAccess = true))
	ACharacter * hittingActor;

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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	FTimerHandle gravityHandle;

	//공중에서 맞았을 때 체공시간을 높이기 위해 낮춰질 중력값
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	float upperGravity{ 0.9f };

	//중력이 낮게 조정되고 다시 원래 중력값으로 되돌아가기 까지의 시간
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	float gravityTimerInRate = 0.3f;

	//하단판정을 가려낼 본이름, 인간형, 로봇형마다 센터 본 이름이 다를수도있어서 일단 변수로지정
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Launch Value", meta = (AllowPrivateAccess = true))
	FName centerBone {"pelvis"};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "World Data", meta = (AllowPrivateAccess = true))
	UCGameInstance * gameInstance;

////////////////////////////////////////////////////////////////////////////////
public://Debug Property
	
};


/*UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);*/

/*UFUNCTION()
	void TakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);*/
