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

//CCharacterStruct�� ���ϰ� �̿��ϱ����� define, Character�Ļ� Ŭ���������� ����Ѵ�.
#define GET_STATE(x) characterStructs->Get##x()
#define SET_STATE(x,y) characterStructs->Set##x##_##y()

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
	//Foot sliding�� ���� ���� ����ȭ ���ǵ�
	float animWalkSpeed{ 0.0f };
	float animRunSpeed{ 0.0f };

	//���� �� Ŀ�� ��� ���ǵ� �ܰ迡 ���� ���� ���Ѵ�
	//Walk = 0, Run = 1
	//MoveMode�� ���� (x)Acceleration, (y)Deceleration ���� ���� ����Ŀ��
	UPROPERTY()
	UCurveVector* movementCurve;
	//rotation interpSpeed�� ���� ���� Ŀ��
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
	class UCReactionComponent * reactionComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	class UCParticleManagerComponent * particleManagerComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	UAnimInstance * animnInst;

////////////////////////////////////////////////////////////////////////////////
public://Delegate

	/**���������� AnimInstance�� bool bIsJump�� �ٲٱ� ���� ���˴ϴ�. */
	FMovement OnJump;
	/**���������� AnimInstance�� bool bIsJump�� �ٲٱ� ���� ���˴ϴ�. */
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

	/* Ư�� �κ����� ������� �ʰ� ���� ��� Ȯ���ؼ� SubState�� �����ؾ��ϴ� ����� ó���� �ô´�.*/
	void UpdateSubState();

	void UpdateRagDoll();

	/* Movement���� ������Ʈ (acc, deacc, maxWalkSpeed, rotationRate..)*/
	void UpdateMovementSetting();

		/** currentMovementSettings�� ������Ʈ �մϴ�. maxWalkSpeed�� eMoveMode�� ���� ����*/
		void UpdateCurrentMovementSettings();

		void UpdateGroundRotation();


	/**
	 * �����̼� �����Ҷ� ���
	 * ex) wall run�� ���� �˸°� �ٴ´ٴ���, ������ Ŀ�갪�̿��� �����Ѵٴ���
	 */
	void UpdateSetLocation();

		/** UpdateSetLocation()�� ���Ǹ� ���Ŭ�������� �������Ͽ� ����Ѵ�. */
		virtual void SetLocation() {};


	/*�÷��̾��� ȸ���� �ڿ������� �ϱ� ���� �Լ�[Ground friction������ ȣ��]*/
	void UpdateRotation();

	/*�������� �����س��µ� ���*/
	void UpdateMovementPreviousValue();

////////////////////////////////////////////////////////////////////////////////
public://Get & Set

	/**
	 *	AnimBlueprint�� ���� �Ѱ��ֱ� ���� �Լ�
	 *
	 *	@param location		- ��ġ
	 *	@param rotation		- ȸ��
	 *	@param velocity		- �ӵ�
	 *	@param speed		- �ӵ�
	 *	@param AnimMaxWalkSpeed	- �ִϸ��̼��� walk �ִ� �ӵ�
	 *	@param AnimMaxRunSpeed	- �ִϸ��̼��� run �ִ� �ӵ�
	 *	@param multiSpeed	- ĳ������ ������ ��� ����
	 *	@param finalAcc		- �������� �ƴ� ���������� ������ acceleration
	 *	@param DistanceAcceleration - DistanceAcceleration�� ����
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

	/** Character�� State(������)���� ��ȯ�մϴ� */
	UFUNCTION(BlueprintCallable, Category = "Get CharacterInfo")
	void GetCharacterState(EMainState& mainState_out, ESubState& subState_out, EMoveMode& movemode_out);

	UFUNCTION(BlueprintPure, Category = "Get CharacterInfo")
	UCCharacterStruct * GetCharacterStruct() { return characterStructs; }

	UFUNCTION(BlueprintPure, Category = "Get CharacterInfo")
	EMainState GetMainState() { return characterStructs->GetMainState(); }

	UFUNCTION(BlueprintPure, Category = "Get CharacterInfo")
	ESubState GetSubState() { return characterStructs->GetSubState(); }

	void SetLastRotationMode(ERotationMode mode);

	UFUNCTION(BlueprintCallable, Category = "Get Component")
	UCReactionComponent * GetReactionComponent() { return reactionComponent; }

////////////////////////////////////////////////////////////////////////////////
protected://member function

	FRotator SmoothCharacterRotation(float targetYaw, const float targetInterpSpeed, const float actorInterpSpeed);
	
	/**���� ���ǵ带 ���ؼ� ����ȭ�� ������ ��Ÿ�� *Movement�� ���� curveValue�� ��µ� ���˴ϴ�.*/
	float GetMappedSpeed();

	float CalculateGroundedRotationRate();

	/**
	 * UpdateGroundedRotation���� �߰������� ������ Rotation�� �ϰ���� �� Override �Ͽ� ���
	 * �� Super�� ��������־�� �մϴ�. Super���� Ture�� ������ return�ϼ���.
	 * @return - �� �ȿ� �߰��� Rotation�� ����Ǿ��ٸ� True, true�� ��� 
	 *			 UpdateGroundedRotation�� �� ��� Return�� �����մϴ�.
	 */
	virtual bool AddUpdateGroundedRotation();

	void PutBackLastRotationMode();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	void RagDollStart();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	void RagDoll_SetCapusleLoc();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	void RagDollEnd();

	UFUNCTION(Category = "Rag Doll")
	void RagDollStartTimer();

	UFUNCTION(Category = "Rag Doll")
	void RagDollEndTimer();

	UFUNCTION(BlueprintCallable, Category = "Rag Doll")
	virtual UAnimMontage * GetGetUpAnimaMontage(bool isFaceUp) { return nullptr; };

////////////////////////////////////////////////////////////////////////////////
protected://member property

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Essential Value")
	UCCharacterStruct * characterStructs;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Essential Value")
	float aimYawRate;
	
	//���� Velocity�� ���� velocity�� ���� ����
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Essential Value")
	FVector acceleration { FVector::ZeroVector };

	//X���� �������� �ٶ󺸴� 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	FVector virtualAcceleration { FVector::ZeroVector };

	//���� �������� Velocity
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	FVector previousVelocity;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	FVector currentVelocity;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement Value")
	float currentSpeed;

	//�ӵ��� �󸶳� ���������� ex) 1.5��� -> 1.5f
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement Value")
	float multiplySpeed;

	MovementSettings currentMovementSettings;

	//�����̴µ��ȿ��� ����Ǵ� Velocity����� rotation
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Previous Value")
	FRotator LastVelocityRotation;

	//�̵�Ű�� ���������� ����Ǵ� Acceleration����� rotation, LastVelocityRotation�ʹ� Ȯ���� �ٸ�����
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	FName hittedActionBone{ "spine_02" };

	//Ragdoll�� �ڿ������� �����ϱ� ���� �ڵ�
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	FTimerHandle endRagdollHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ragdoll Value")
	float ragDollWeight{ 0.f };

	//RagDoll���¿��� �Ͼ �� �ִ��� 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Value")
	bool bCanGetUp{ false };

	/*	������ ��ġ�� ���� Pelvis�� Z��ġ�� �޶����⿡ Ground�϶� ���� ����ִ� �κа� ĸ�� ���� ���̸� ����
	 *	��ġ���� Pelvis�� ��ġ�� ���� LayDown�� �Ǻ��� Z���� ������ ���մϴ�.
	 *	���� 0.2��� ĸ������ ��ġ�������� 0.2������ŭ ������ ���� �Ǻ���ġ�� �˴ϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "State Value")
	float ratioDeterminLayDownZ{ 0.5f };
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Character Information")
	float halfCapusleHeight{ 0.f };

private:

	float deltaTime;

	const float gravityScale{ 3.f };

	ERotationMode lastRotationMode;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float attackRotInterpSpeed{ 50.0f };
};

