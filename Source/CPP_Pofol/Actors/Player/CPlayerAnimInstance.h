#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Structs/Character/CCharacterStruct.h"
#include "CPlayerAnimInstance.generated.h"

#define DEBUG_ANIM if(bDebugMode == true)

class ACPlayer;

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

UENUM(BlueprintType)
enum class ETurn180 : uint8
{
	LEFT,RIGHT,MAX
};

class ACPlayer;

UCLASS()
class CPP_POFOL_API UCPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////
public://Default Event
	
	virtual void NativeBeginPlay() override;

////////////////////////////////////////////////////////////////////////////////
public://Delegate

	/**
	*Owner Character�� ���������� ����˴ϴ�.
	*/
	UFUNCTION()
		void OnJump_Animinstance();
	/**
	*Owner Character�� ���������� ����˴ϴ�.
	*/
	UFUNCTION()
		void OnLand_Animinstance();

////////////////////////////////////////////////////////////////////////////////
protected: //Owner Value

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		float OwnerSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		float OwnerMaxSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		FVector OwnerVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		float OwnerDirection = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		FVector OwnerAcceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		FRotator OwnerRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		FVector OwnerLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		FVector OwnerForwardVecter = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		ERotationMode OwnerRotationMode = ERotationMode::VELOCITY;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "OwnerValue")
		bool bIsJump = false;

	//Ŭ����
	ACPlayer * owner;

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Idle")
		bool isIdle = false;

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Aimoffset_Idle")
		FRotator Aimoffset = FRotator::ZeroRotator;

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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "IdleTurn")
		float IdleTurnTimer = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "IdleTurn")
		float MaxTurnTime = 2.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "IdleTurn")
		bool isIdleTurn = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "IdleTurn")
		float idleTurnYaw = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "IdleTurn")
		float idleTurnLastYaw = 0.0f;


protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MoveStart")
		float PlayerMoveStartYaw = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MoveStart")
		FVector PlayerMoveStartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MoveStart")
		FRotator RotationLastTick;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MoveStart")
		float MoveStartAngleDistance = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MoveStart")
		bool isMoveStart = false;

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MoveStop")
		bool isMoveStop = false;

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Turn180")
		bool isTurn180 = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Turn180")
		float lastRotationCurveValue = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Turn180")
		float PlayerTurn180StartYaw = 0.0f;


////////////////////////////////////////////////////////////////////////////////
protected://Foot IK

	/**
	*���� foot IK�� �����Ϳ� Add�� Transform�� ���ϰ� �� Transform�� ������ Transform�� ���̸� ����
	*@param EnalbeFootIK - footIK�� Ȱ��ȭ ��
	*@param IKFootBone - foot Ik���� �̸�
	*@param RootBone - ��Ʈ���̸�
	*@param CurrentLocationTarget -
	*@param CurrentLocationOffset - footIK�� ������ Transform�� Location
	*@param CurrentRotationOffset - footIK�� ������ Transform�� Rotation
	*/
	UFUNCTION(BlueprintCallable)
		void SetFootOffset(bool EnalbeFootIK, FName IKFootBone, FName RootBone, UPARAM(ref) FVector & CurrentLocationTarget, UPARAM(ref) FVector & CurrentLocationOffset, UPARAM(ref) FRotator & CurrentRotationOffset);

	UFUNCTION(BlueprintCallable)
		void SetPelvisIKOffset(bool EnalbeFootIK, FVector FootOffset_L_Target, FVector FootOffset_R_Target);

	/**
	*FootLock Transform�� Update
	*@param enalbeFootIK - footIk�� Ȱ��ȭ �Ǿ��ִ����� (������ ĳ���� ȸ�� ������ Ŀ��� ��� �������µ�						 ���� ȸ�� ���ҰŶ� bool������ ���
	*@param footLockCurve - 
	*@param 
	*@param CurrentLocationTarget -
	*@param CurrentLocationOffset - footIK�� ������ Transform�� Location
	*@param CurrentRotationOffset - footIK�� ������ Transform�� Rotation
	*/
	UFUNCTION(BlueprintCallable)
		void SetFootLocking(bool enableFootIK, FName footLockCurve, FName ikFootBone, 
			UPARAM(ref) float & currentFootLockAlpha, UPARAM(ref) FVector & currentFootLockLocation, UPARAM(ref)FRotator & currentFootLockRotation);

	UFUNCTION(BlueprintCallable)
		void SetFootLockOffset( UPARAM(ref) FVector localLocation, UPARAM(ref) FRotator localRotation);

protected:

	//���� ���� ���� �����°� �����ϱ����� �÷��ִ� ��
	float FootHeight = 25.5f;
	//float FootHeight = 500.5f;

	float test = 33.0f;

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


////////////////////////////////////////////////////////////////////////////////
public://Debug
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Foot IK")
	bool bDebugMode{ false };
};