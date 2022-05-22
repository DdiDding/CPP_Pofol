#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/CESkillType.h"
#include "CCharacterAnimInstance.generated.h"

#define DEBUG_CCHAR_ANIM if(bDebugMode == true)

class ACCharacter;
enum class EMainState : uint8;
enum class ESubState : uint8;
enum class EMoveMode : uint8;
enum class ERotationMode : uint8;

//enum class EReactionType : uint8;//이것만 안됨 ㅠ

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float F;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float B;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float L;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float R;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FB;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float LR;
};

/**
 * 
 */
UCLASS()
class CPP_POFOL_API UCCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

////////////////////////////////////////////////////////////////////////////////
public://virtual function

	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


////////////////////////////////////////////////////////////////////////////////
public://Delegate

	/** Owner Character가 점프했을때 실행됩니다. */
	UFUNCTION()
	void OnJump_Animinstance();

	/** Owner Character가 착지했을때 실행됩니다. */
	UFUNCTION()
	void OnLand_Animinstance();

	/** Owner Character가 공격 당했을때 ReactionComponent에서 실행됩니다. */
	UFUNCTION()
	void OnHitted_Animinstance(EReactionType data);


////////////////////////////////////////////////////////////////////////////////
protected://Update function

	void UpdateOwnerValue();
	void UpdateDefaultValue();
	virtual void UpdateGround_Movement();
	virtual void UpdateGround_Idle();
	virtual void UpdateAir();
	virtual void UpdateSubState();
	virtual void UpdateFootIK();

////////////////////////////////////////////////////////////////////////////////
protected://member function

	/*********************************************************************************************
	* Default Function
	********************************************************************************************* */
	/* Hitted될때 실행되는 OnHitted_Animinstance의 내부에서 동작합니다. 각 자손 클래스마다 알아서 처리하길*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Default Function")
	void OnHitted_Animinstance_Internal();


	/*********************************************************************************************
	* Update Movement Ground
	********************************************************************************************* */

	//F,B,L,R 어느 방향으로 진행중인지 값을 저장합니다. 진행중이면 1의 값, 아니면 0의 값을 가집니다.
	UFUNCTION(BlueprintCallable, Category = "UpdateMovementGround")
	void CalculateVelocityBlend();

	UFUNCTION(BlueprintCallable, Category = "UpdateMovementGround")
	void CalculateRelativeAccelerationAmount();

	UFUNCTION(BlueprintCallable, Category = "UpdateMovementGround")
	void CalculateWalkRunBlend();

	UFUNCTION(BlueprintCallable, Category = "UpdateMovementGround")
	void CalculateStrideBlend();

	UFUNCTION(BlueprintCallable, Category = "UpdateMovementGround")
	void CalculateStandingPlayRate();

	/*********************************************************************************************
	* Foot IK
	********************************************************************************************* */


	/**
	 *한쪽 foot IK의 이펙터에 Add될 Transform을 정하고 이 Transform과 미적용 Transform의 차이를 구함
	 *
	 *@param EnalbeFootIK			- footIK의 활성화 값
	 *@param IKFootBone				- foot Ik본의 이름
	 *@param RootBone				- 루트본이름
	 *@param CurrentLocationTarget	- 최종 발이 닿을 World상 Z값(x,y제외) 왼,오른쪽 이 값의 차이를 통해 
									  Pelvis의 위치를 구한다 
	 *@param CurrentLocationOffset	- footIK의 이펙터 Transform의 Location
	 *@param CurrentRotationOffset	- footIK의 이펙터 Transform의 Rotation
	 */
	UFUNCTION(BlueprintCallable)
	void SetFootOffset(FName EnableCurveName, FName IKFootBone, FName RootBone, FVector & CurrentLocationTarget, FVector & CurrentLocationOffset, FRotator & CurrentRotationOffset);

	UFUNCTION(BlueprintCallable)
	void SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target);

	/**
	*FootLock Transform을 Update
	*@param enalbeFootIK - footIk가 활성화 되어있는지값 (원래는 캐릭터 회전 떄문에 커브로 양발 나누었는데	나는 회전 안할거라서 bool값으로 사용
	*@param EnableCurveName			- footIK가 적용 될때만 Locking이 적용될수도있기에 커브값 이름
	*@param footLockCurve -
	*@param CurrentLocationTarget	-
	*@param CurrentLocationOffset	- footIK의 이펙터 Transform의 Location
	*@param CurrentRotationOffset	- footIK의 이펙터 Transform의 Rotation
	*/
	UFUNCTION(BlueprintCallable, Category = "Foot Locking")
	void SetFootLocking(FName EnableCurveName, FName footLockCurve, FName ikFootBone,
		float & currentFootLockAlpha, FVector & currentFootLockLocation, 
		FRotator & currentFootLockRotation);

	UFUNCTION(BlueprintCallable, Category = "Foot Locking")
	void SetFootLockOffset(FVector& localLocation, FRotator& localRotation);

	float GetAnimcurveClamped(FName curveName, float bias, float clampMin, float clampMax);


	/*********************************************************************************************
	* Get & Set
	********************************************************************************************* */
public:

	UFUNCTION(BlueprintCallable, Category = "Set Function")
	void SetIsHitted_False() { bIsHitted = false; };
	UFUNCTION(BlueprintCallable, Category = "Set Function")
	void SetIsHitted_True() { bIsHitted = true; };

////////////////////////////////////////////////////////////////////////////////
protected://member Value

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	ACCharacter * owner;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	FVector ownerLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	FRotator ownerRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	FVector ownerVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	float ownerSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	float ownerMulSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	FVector ownerAcceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	FVector ownerDistanceAcc = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	float ownerDirection = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	FVector ownerForwardVecter = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	float ownerAnimatedWalkSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	float ownerAnimatedRunSpeed;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "OwnerValue")
	EMainState ownerMainState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "OwnerValue")
	ESubState ownerSubState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "OwnerValue")
	EMoveMode ownerMoveMode;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "OwnerValue")
	ERotationMode ownerRotationMode;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
	bool bIsJump = false;

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UpdateMovementGround")
	FVelocityBlend velocityBlend;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UpdateMovementGround")
	float velBlendInterpSpeed{ 12.0f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UpdateMovementGround")
	FVector relativeAccelerationAmount;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UpdateMovementGround")
	FLeanAmount leanAmount;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UpdateMovementGround")
	float leanAmountInterpSpeed{ 15.0f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UpdateMovementGround")
	float walkRunBlend{ 0.0f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UpdateMovementGround")
	float strideBlend{ 0.0f };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UpdateMovementGround")
	float standingPlayRate{ 0.0f };

protected:

	//공통적으로 사용할 수 있는 커브
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Blend Curves")
	UCurveFloat * strideBlendNormal;


protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue_Previous")
		FVector PreOwnerVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue_Previous")
		FVector PreOwnerAcceleration = FVector::ZeroVector;

protected:
	//플레이어의 정면과 벨로시티의 yaw 차이
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Move")
		float YawDistance = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Move")
		bool isMove = false;

protected:

	/**	
	 *	Foot IK가 발목의 위치를 통해 하는거라서 발목을 땅에 붙일수는 없으니까 
	 *	FootHegiht값 만큼 더해서 발바닥에 붙는것처럼 보이게 하는 값이다.
	 */
	float FootHeight { 15.5f };

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	bool bEnableFootIK { false };

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FVector FootOffset_L_Location = FVector::OneVector;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FVector FootOffset_R_Location = FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FRotator FoorOffset_L_Rotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FRotator FoorOffset_R_Rotation = FRotator::ZeroRotator;

	//Pelvis
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FVector PelvisOffset = FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	float pelvisAlpha{ 0.0f };

	//footLock
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FVector FootLock_L_Location = FVector::OneVector;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FVector FootLock_R_Location = FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FRotator FootLock_L_Rotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	FRotator FootLock_R_Rotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	float FootLock_L_Alpha = 0.0f;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Foot IK")
	float FootLock_R_Alpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Hit Value")
	bool bIsHitted{ false };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Hit Value")
	EReactionType reactionType; //이것만 안됨 ㅠ


////////////////////////////////////////////////////////////////////////////////
public://Debug & World

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Debug")
	bool bDebugMode{ false };

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default Value")
	float deltaTime{ 0.0f };

	//몽타주의 실행시간의 비율
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default Value")
	float montagePlayTimeInRatio{ 0.0f };
};
