#include "CPlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CWallRunComponent.h"
#include "Util/CLog.h"
#include "Actors/Player/CPlayer.h"
#include "DrawDebugHelpers.h"

void UCPlayerAnimInstance::NativeBeginPlay()
{
	owner = Cast<ACPlayer>(this->TryGetPawnOwner());

	//ACCharacter��ȯ �����ϸ� ��������Ʈ ����
	ACPlayer* tempOnwer = Cast<ACPlayer>(owner);
	if (tempOnwer != nullptr)
	{
		tempOnwer->OnJump.BindUFunction(this, FName("OnJump_Animinstance"));
		tempOnwer->OnLand.BindUFunction(this, FName("OnLand_Animinstance"));
		tempOnwer->GetWallRunComponent()->OnWallJump.BindUFunction(this, FName("OnJump_Animinstance"));
		tempOnwer->GetWallRunComponent()->OnWallStart.BindUFunction(this, FName("OnLand_Animinstance"));
	}
}

void UCPlayerAnimInstance::OnJump_Animinstance()
{
	bIsJump = true;
	return;
}

void UCPlayerAnimInstance::OnLand_Animinstance()
{
	bIsJump = false;
	return;
}

////////////////////////////////////////////////////////////////////////////////
//Foot IK

//CurrentLocationTarget : ���� ���� ���� Z��
void UCPlayerAnimInstance::SetFootOffset(bool EnalbeFootIK,
	FName IKFootBone, FName RootBone, UPARAM(ref) FVector & CurrentLocationTarget,
	UPARAM(ref) FVector & CurrentLocationOffset, UPARAM(ref) FRotator & CurrentRotationOffset
)
{
	//Step 1: IKBone ������ Trace�� ��Ƽ� Impact Location�� Normal�� �ӽ�����
	USkeletalMeshComponent * ownerSkeletal = GetOwningComponent();

	APawn * OwnerCharacter = TryGetPawnOwner();

	FVector startTraceLocation = FVector::ZeroVector;
	FVector endTraceLocaiton = FVector::ZeroVector;
	FVector IKFootFloorLocation = FVector::ZeroVector;
	FVector ImpactPoint = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;

	FRotator TargetRotationOffset = FRotator::ZeroRotator;

	float deltaSec = GetWorld()->DeltaTimeSeconds;

	float IKTraceDistance = 50.0f;
	float IKTraceDistance_low = 25.0f;

	//IK���������� Ȯ��
	if (EnalbeFootIK == false) 
	{
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
		return;
	}

	//IK������� �߼����� ��ġ
	IKFootFloorLocation = ownerSkeletal->GetSocketLocation(IKFootBone);
	IKFootFloorLocation.Z = ownerSkeletal->GetSocketLocation(RootBone).Z;

	startTraceLocation = IKFootFloorLocation;
	startTraceLocation.Z += IKTraceDistance;

	endTraceLocaiton = IKFootFloorLocation;
	endTraceLocaiton.Z -= IKTraceDistance_low;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(TEXT("ikTrace"), false, OwnerCharacter);
	
	//�浹�Ѱ� ������ if�� ����
	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		startTraceLocation,
		endTraceLocaiton,
		ECollisionChannel::ECC_Visibility,
		TraceParams) == false)
	{
		DEBUG_ANIM
		{
			DrawDebugLine(GetWorld(), startTraceLocation, endTraceLocaiton, FColor::Red, false, -1.0f, 0, 5);
		}
		return;
	}

	//LineTrace �����
	{
		DEBUG_ANIM
		{
			DrawDebugLine(GetWorld(), startTraceLocation, endTraceLocaiton, FColor::Green, false, -1.0f, 0, 5);
			DrawDebugSolidBox(GetWorld(), HitResult.ImpactPoint, FVector(10.f), FColor::Blue, false, -1, 0);
		}
	}

	ImpactPoint = HitResult.ImpactPoint;
	ImpactNormal = HitResult.ImpactNormal;


	//Step 1.1 IK����Ǿ� ���� ���� ���� Z��(X,Y����) ,
	//FootHeight�� �ڿ��������ϱ����� ���ε� ���� �߸�
	CurrentLocationTarget = (ImpactPoint + (ImpactNormal * FootHeight)) - (IKFootFloorLocation + (FVector(0, 0, FootHeight)));

	//Step 1.2 ���� ���鿡 ���� ȸ���� ��
	TargetRotationOffset = FRotator(
		FMath::RadiansToDegrees(atan2(ImpactNormal.X, ImpactNormal.Z) * -1.0f),
		0,
		FMath::RadiansToDegrees(atan2(ImpactNormal.Y, ImpactNormal.Z)));


	//Step 2 ������ġ�� ������ġ ������
	CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset, CurrentLocationTarget, deltaSec, 30.0f);
	
	//Step 3
	//����ȸ���� ����ȸ�� ������
	CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset, TargetRotationOffset, deltaSec, 30.0f);

}

void UCPlayerAnimInstance::SetPelvisIKOffset(bool EnalbeFootIK, FVector FootOffset_L_Target, FVector FootOffset_R_Target)
{
	FVector PelvisTarget = FVector::ZeroVector;
	float deltaSec = GetWorld()->DeltaTimeSeconds;


	//IK���������� Ȯ��
	if (EnalbeFootIK == false)
	{
		this->PelvisOffset = FVector::ZeroVector;
		return;
	}


	//Step 1 �㸮�� ���̸� ������ �� ���� ��ġ�� ���� ��ġ ����
	if (FootOffset_L_Target.Z < FootOffset_R_Target.Z)
	{
		PelvisTarget = FootOffset_L_Target;
	}
	else {
		PelvisTarget = FootOffset_R_Target;
	}

	//Step 2 
	this->PelvisOffset = FMath::VInterpTo(PelvisOffset, PelvisTarget, deltaSec, 10.0f);
}

void UCPlayerAnimInstance::SetFootLocking(bool enableFootIK, FName footLockCurve, FName ikFootBone, UPARAM(ref)float& currentFootLockAlpha, UPARAM(ref)FVector& currentFootLockLocation, UPARAM(ref)FRotator& currentFootLockRotation)
{
	if (owner == nullptr) return;
	
	//foot IKȰ��ȭ �Ǵ�
	if (enableFootIK == false)
	{
		currentFootLockAlpha = 0.0f;
		return;
	}

	//footLock�� �ӽ�����
	float footLocakCurveValue = this->GetCurveValue(footLockCurve);


	//2�ܰ�: footLocakCurve���� ���� alpha�� ���� �۰ų� 1�� ��쿡�� FootLock Alpha�� ������Ʈ�մϴ�. �̷��� �ϸ� ���� ��� ��ġ���� ����ų� �� ��ġ�� ��� ���� �ְ� ���� ������ �ʵ��� �մϴ�.
	//footLocakCurveValue�� �ڿ������� �ö󰡰ų�, �ڿ������� �������ų�, �ٷ� 1�� �Ǵ� ������ �� �̵��� �ִ�
	//�� ���ǽ��� currentFootLockAlpha�� �ٷ� 1�� �ǰų� �ڿ������� ���������� ������ �ڿ������� �ö󰡰� ������ �ʽ��ϴ�. ��� �ٷ� 1�� �ǰų� 1���� �ڿ������� �������ų� ���� �ϳ��Դϴ�.
	if (0.99f <= footLocakCurveValue || footLocakCurveValue < currentFootLockAlpha)
	{
		currentFootLockAlpha = footLocakCurveValue;
	}

	//currentFootLockAlpha�� 1�� �ɶ� ���� pose�� ���ص� �ִ��� ���� transform�� 
	//component�������� �����մϴ�.
	if (0.99f < currentFootLockAlpha)
	{
		FTransform tempTransform = this->GetOwningComponent()->GetSocketTransform(ikFootBone,ERelativeTransformSpace::RTS_Component);
		currentFootLockLocation = tempTransform.GetLocation();
		currentFootLockRotation = tempTransform.GetRotation().Rotator();
	}

	if (0.0f < currentFootLockAlpha)
	{
		SetFootLockOffset(currentFootLockLocation, currentFootLockRotation);
	}

	return;
}

void UCPlayerAnimInstance::SetFootLockOffset(UPARAM(ref)FVector localLocation, UPARAM(ref)FRotator localRotation)
{

	FVector locationDifference;
	FRotator rotationDifference;

	//���� ȸ���� ���������� ������Ʈ�� ȸ�� ������ ��Ÿ�� ����Ͽ� ���鿡 ������ ���¸� �����ϱ� ���� ���� �󸶳� ȸ���ؾ� �ϴ��� ã���ϴ�.
	if (owner->GetCharacterMovement()->IsMovingOnGround() == true)
	{
		rotationDifference = OwnerRotation - owner->GetCharacterMovement()->GetLastUpdateRotation();
	}
	
	//�޽� ȸ���� �������� ������ ���̸� �̵��� �Ÿ��� ������ ���鿡 ������ ���·� �����ϱ� ���� ���� �����µǾ�� �ϴ� ������ ã���ϴ�.
	locationDifference = this->GetOwningComponent()->GetComponentRotation().UnrotateVector(OwnerVelocity * GetWorld()->DeltaTimeSeconds);

	//���� ���� ��ġ���� ��ġ ���̸� ���� ȸ�� ���̸�ŭ ȸ���Ͽ� ���� ������� ������ �����ǵ��� �մϴ�.
	FVector tempLoc = localLocation - locationDifference;
	tempLoc.RotateAngleAxis(rotationDifference.Yaw, FVector(0, 0, -1.0f));
	localLocation = tempLoc;

	//�� ���� ȸ���� �������� ���� ���� ȸ������ ȸ�� ���̸� ���ϴ�.
	localRotation = localRotation - rotationDifference;

	return;
}

