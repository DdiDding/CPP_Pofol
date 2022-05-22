#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Structs/Character/CCharacterStruct.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveFloat.h"
#include "DrawDebugHelpers.h"
#include "CCharacter.generated.h"

//CCharacterStruct를 편하게 이용하기위한 define, Character파생 클래스에서만 사용한다.
#define GET_STATE(x) characterStructs23->Get##x()
#define SET_STATE(x,y) characterStructs23->Set##x##_##y()

class UCDA_WeaponData;
enum class ERotationMode : uint8;

DECLARE_DELEGATE(FMovement);

UCLASS()
class CPP_POFOL_API ACCharacter : public ACharacter
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////
//Inner Struct
struct MovementSettings
{
	//Foot sliding을 막기 위한 동기화 스피드
	float animWalkSpeed{ 0.0f };
	float animRunSpeed{ 0.0f };

	//밑의 두 커브 모두 스피드 단계에 따라 값을 정한다
	//Walk = 0, Run = 1
	//MoveMode에 따른 (x)Acceleration, (y)Deceleration 값을 가진 벡터커브
	UPROPERTY()
	UCurveVector* movementCurve;
	//rotation interpSpeed의 값을 가진 커브
	UPROPERTY()
	UCurveFloat* rotationRateCurve;
};

////////////////////////////////////////////////////////////////////////////////
public://Virtual Func

	ACCharacter();

protected:

	virtual void PostInitializeComponents() final;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnJumped_Implementation() final;

	virtual void Landed(const FHitResult & Hit) final;
	virtual void Falling() final;

////////////////////////////////////////////////////////////////////////////////
protected://Components

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	class UCSkillManagerComponent * skillManagerComponent7;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	class UCWeaponManagerComponent * weaponManagerComponent8;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	class UCReactionComponent * reactionComponent1;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	class UCParticleManagerComponent * particleManagerComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	UAnimInstance * animnInst;

////////////////////////////////////////////////////////////////////////////////
public://Delegate

	/**점프했을때 AnimInstance의 bool bIsJump를 바꾸기 위해 사용됩니다. */
	FMovement OnJump;
	/**착지했을때 AnimInstance의 bool bIsJump를 바꾸기 위해 사용됩니다. */
	FMovement OnLand;


////////////////////////////////////////////////////////////////////////////////
public://Hand IK

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HandIK")
	FVector locHandIk_l;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HandIK")
	FVector locHandIk_r;

////////////////////////////////////////////////////////////////////////////////
protected://Use the update

	void UpdateEssentialValue();

	/* 특정 부분으로 변경되지 않고 따로 계속 확인해서 SubState를 변경해야하는 경우의 처리를 맡는다.*/
	void UpdateSubState();

	void UpdateRagDoll();

	/* Movement관련 업데이트 (acc, deacc, maxWalkSpeed, rotationRate..)*/
	void UpdateMovementSetting();

		/** currentMovementSettings을 업데이트 합니다. maxWalkSpeed를 eMoveMode에 따라 설정*/
		void UpdateCurrentMovementSettings();

		void UpdateGroundRotation();


	/**
	 * 로케이션 조절할때 사용
	 * ex) wall run에 벽에 알맞게 붙는다던지, 파쿠르때 커브값이용해 세팅한다던지
	 */
	void UpdateSetLocation();

		/** UpdateSetLocation()에 사용되며 상속클래스에서 재정의하여 사용한다. */
		virtual void SetLocation() {};


	/*플레이어의 회전을 자연스럽게 하기 위한 함수[Ground friction때문에 호출]*/
	void UpdateRotation();

	/*이전값을 저장해놓는데 사용*/
	void UpdateMovementPreviousValue();

////////////////////////////////////////////////////////////////////////////////
public://Get & Set

	/**
	 *	AnimBlueprint로 값을 넘겨주기 위한 함수
	 *
	 *	@param location		- 위치
	 *	@param rotation		- 회전
	 *	@param velocity		- 속도
	 *	@param speed		- 속도
	 *	@param AnimMaxWalkSpeed	- 애니메이션의 walk 최대 속도
	 *	@param AnimMaxRunSpeed	- 애니메이션의 run 최대 속도
	 *	@param multiSpeed	- 캐릭터의 빠르기 배속 정도
	 *	@param finalAcc		- 보간값이 아닌 최종적으로 가야할 acceleration
	 *	@param DistanceAcceleration - DistanceAcceleration과 같음
	 */
	UFUNCTION(BlueprintCallable, Category = "Get CharacterInfo")
	void GetCharacterMovementValue(
		FVector& location, 
		FRotator& rotation, 
		FVector& velocity, 
		float& speed,
		float& animMaxWalkSpeed,
		float& animMaxRunSpeed, 
		float& multiSpeed, 
		FVector& finalAcc,
		FVector& DistanceAcc
	);

	/** Character의 State(열거형)들을 반환합니다 */
	UFUNCTION(BlueprintCallable, Category = "Get CharacterInfo")
	void GetCharacterState(EMainState& mainState_out, ESubState& subState_out, EMoveMode& movemode_out);

	UFUNCTION(BlueprintPure, Category = "Get CharacterInfo")
	UCCharacterStruct * GetCharacterStruct() { return characterStructs23; }

	UFUNCTION(BlueprintPure, Category = "Get CharacterInfo")
	EMainState GetMainState() { return characterStructs23->GetMainState(); }

	UFUNCTION(BlueprintPure, Category = "Get CharacterInfo")
	ESubState GetSubState() { return characterStructs23->GetSubState(); }

	void SetLastRotationMode(ERotationMode mode);

	UFUNCTION(BlueprintCallable, Category = "Get Component")
	UCReactionComponent * GetReactionComponent() { return reactionComponent1; }

////////////////////////////////////////////////////////////////////////////////
protected://member function

	FRotator SmoothCharacterRotation(float targetYaw, const float targetInterpSpeed, const float actorInterpSpeed);
	
	/**현재 스피드를 구해서 정규화한 값으로 나타냄 *Movement에 따른 curveValue를 얻는데 사용됩니다.*/
	float GetMappedSpeed();

	float CalculateGroundedRotationRate();

	/**
	 * UpdateGroundedRotation에서 추가적으로 조건적 Rotation을 하고싶을 때 Override 하여 사용
	 * 꼭 Super을 실행시켜주어야 합니다. Super에서 Ture가 나오면 return하세요.
	 * @return - 이 안에 추가한 Rotation이 실행되었다면 True, true일 경우 
	 *			 UpdateGroundedRotation는 그 즉시 Return을 수행합니다.
	 */
	virtual bool AddUpdateGroundedRotation();

	void PutBackLastRotationMode();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	void RagDollStart();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	void RagDoll_SetCapusleLoc();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	void RagDollEnd();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	virtual UAnimMontage * GetGetUpAnimaMontage(bool isFaceUp) { return nullptr; };

////////////////////////////////////////////////////////////////////////////////
protected://member property

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Essential Value")
	UCCharacterStruct * characterStructs23;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Essential Value")
	float aimYawRate;
	
	//현재 Velocity와 이전 velocity의 차이 벡터
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Essential Value")
	FVector acceleration { FVector::ZeroVector };

	//X축을 정면으로 바라보는 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	FVector virtualAcceleration { FVector::ZeroVector };

	//이전 프레임의 Velocity
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	FVector previousVelocity;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	FVector currentVelocity;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	float currentSpeed;

	//속도가 얼마나 빨라질건지 ex) 1.5배속 -> 1.5f
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement Value")
	float multiplySpeed;

	MovementSettings currentMovementSettings;

	//움직이는동안에만 저장되는 Velocity기반의 rotation
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Previous Value")
	FRotator LastVelocityRotation;

	//이동키를 눌렀을때만 저장되는 Acceleration기반의 rotation, LastVelocityRotation와는 확실히 다른거임
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Previous Value")
	FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Previous Value")
	float previousAimYaw{ 0.0f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Roatation Value")
	FRotator targetRotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Roatation Value")
	FRotator InAirRotation;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	FVector LastRagDollVelocity { FVector::ZeroVector };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	bool bIsRagdolFaceUp{ false };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	bool bRagdollOnGround{ false };

	//RagDoll상태에서 일어날 수 있는지 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	bool bCanGetUp{ false };
	
private:

	float deltaTime;

	const float gravityScale{ 3.0f };

	ERotationMode lastRotationMode;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float attackRotInterpSpeed{ 50.0f };
};

