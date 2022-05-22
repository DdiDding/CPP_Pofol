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

//enum class EReactionType : uint8;//�̰͸� �ȵ� ��

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

	/** Owner Character�� ���������� ����˴ϴ�. */
	UFUNCTION()
	void OnJump_Animinstance();

	/** Owner Character�� ���������� ����˴ϴ�. */
	UFUNCTION()
	void OnLand_Animinstance();

	/** Owner Character�� ���� �������� ReactionComponent���� ����˴ϴ�. */
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
	/* Hitted�ɶ� ����Ǵ� OnHitted_Animinstance�� ���ο��� �����մϴ�. �� �ڼ� Ŭ�������� �˾Ƽ� ó���ϱ�*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Default Function")
	void OnHitted_Animinstance_Internal();


	/*********************************************************************************************
	* Update Movement Ground
	********************************************************************************************* */

	//F,B,L,R ��� �������� ���������� ���� �����մϴ�. �������̸� 1�� ��, �ƴϸ� 0�� ���� �����ϴ�.
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
	 *���� foot IK�� �����Ϳ� Add�� Transform�� ���ϰ� �� Transform�� ������ Transform�� ���̸� ����
	 *
	 *@param EnalbeFootIK			- footIK�� Ȱ��ȭ ��
	 *@param IKFootBone				- foot Ik���� �̸�
	 *@param RootBone				- ��Ʈ���̸�
	 *@param CurrentLocationTarget	- ���� ���� ���� World�� Z��(x,y����) ��,������ �� ���� ���̸� ���� 
									  Pelvis�� ��ġ�� ���Ѵ� 
	 *@param CurrentLocationOffset	- footIK�� ������ Transform�� Location
	 *@param CurrentRotationOffset	- footIK�� ������ Transform�� Rotation
	 */
	UFUNCTION(BlueprintCallable)
	void SetFootOffset(FName EnableCurveName, FName IKFootBone, FName RootBone, FVector & CurrentLocationTarget, FVector & CurrentLocationOffset, FRotator & CurrentRotationOffset);

	UFUNCTION(BlueprintCallable)
	void SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target);

	/**
	*FootLock Transform�� Update
	*@param enalbeFootIK - footIk�� Ȱ��ȭ �Ǿ��ִ����� (������ ĳ���� ȸ�� ������ Ŀ��� ��� �������µ�	���� ȸ�� ���ҰŶ� bool������ ���
	*@param EnableCurveName			- footIK�� ���� �ɶ��� Locking�� ����ɼ����ֱ⿡ Ŀ�갪 �̸�
	*@param footLockCurve -
	*@param CurrentLocationTarget	-
	*@param CurrentLocationOffset	- footIK�� ������ Transform�� Location
	*@param CurrentRotationOffset	- footIK�� ������ Transform�� Rotation
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

	//���������� ����� �� �ִ� Ŀ��
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Blend Curves")
	UCurveFloat * strideBlendNormal;


protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue_Previous")
		FVector PreOwnerVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue_Previous")
		FVector PreOwnerAcceleration = FVector::ZeroVector;

protected:
	//�÷��̾��� ����� ���ν�Ƽ�� yaw ����
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Move")
		float YawDistance = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Move")
		bool isMove = false;

protected:

	/**	
	 *	Foot IK�� �߸��� ��ġ�� ���� �ϴ°Ŷ� �߸��� ���� ���ϼ��� �����ϱ� 
	 *	FootHegiht�� ��ŭ ���ؼ� �߹ٴڿ� �ٴ°�ó�� ���̰� �ϴ� ���̴�.
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
	EReactionType reactionType; //�̰͸� �ȵ� ��


////////////////////////////////////////////////////////////////////////////////
public://Debug & World

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Debug")
	bool bDebugMode{ false };

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default Value")
	float deltaTime{ 0.0f };

	//��Ÿ���� ����ð��� ����
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default Value")
	float montagePlayTimeInRatio{ 0.0f };
};
