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

	//ACCharacter변환 가능하면 델리게이트 연결
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

//CurrentLocationTarget : 최종 발이 닿을 Z값
void UCPlayerAnimInstance::SetFootOffset(bool EnalbeFootIK,
	FName IKFootBone, FName RootBone, UPARAM(ref) FVector & CurrentLocationTarget,
	UPARAM(ref) FVector & CurrentLocationOffset, UPARAM(ref) FRotator & CurrentRotationOffset
)
{
	//Step 1: IKBone 밑으로 Trace를 쏘아서 Impact Location과 Normal을 임시저장
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

	//IK적용중인지 확인
	if (EnalbeFootIK == false) 
	{
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
		return;
	}

	//IK미적용된 발소켓의 위치
	IKFootFloorLocation = ownerSkeletal->GetSocketLocation(IKFootBone);
	IKFootFloorLocation.Z = ownerSkeletal->GetSocketLocation(RootBone).Z;

	startTraceLocation = IKFootFloorLocation;
	startTraceLocation.Z += IKTraceDistance;

	endTraceLocaiton = IKFootFloorLocation;
	endTraceLocaiton.Z -= IKTraceDistance_low;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(TEXT("ikTrace"), false, OwnerCharacter);
	
	//충돌한게 없으면 if문 실행
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

	//LineTrace 디버깅
	{
		DEBUG_ANIM
		{
			DrawDebugLine(GetWorld(), startTraceLocation, endTraceLocaiton, FColor::Green, false, -1.0f, 0, 5);
			DrawDebugSolidBox(GetWorld(), HitResult.ImpactPoint, FVector(10.f), FColor::Blue, false, -1, 0);
		}
	}

	ImpactPoint = HitResult.ImpactPoint;
	ImpactNormal = HitResult.ImpactNormal;


	//Step 1.1 IK적용되어 발이 닿을 절대 Z값(X,Y제외) ,
	//FootHeight는 자연스럽게하기위한 값인데 아직 잘모름
	CurrentLocationTarget = (ImpactPoint + (ImpactNormal * FootHeight)) - (IKFootFloorLocation + (FVector(0, 0, FootHeight)));

	//Step 1.2 발이 지면에 따라 회전할 값
	TargetRotationOffset = FRotator(
		FMath::RadiansToDegrees(atan2(ImpactNormal.X, ImpactNormal.Z) * -1.0f),
		0,
		FMath::RadiansToDegrees(atan2(ImpactNormal.Y, ImpactNormal.Z)));


	//Step 2 현재위치와 최종위치 보간함
	CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset, CurrentLocationTarget, deltaSec, 30.0f);
	
	//Step 3
	//현재회전과 최종회전 보간함
	CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset, TargetRotationOffset, deltaSec, 30.0f);

}

void UCPlayerAnimInstance::SetPelvisIKOffset(bool EnalbeFootIK, FVector FootOffset_L_Target, FVector FootOffset_R_Target)
{
	FVector PelvisTarget = FVector::ZeroVector;
	float deltaSec = GetWorld()->DeltaTimeSeconds;


	//IK적용중인지 확인
	if (EnalbeFootIK == false)
	{
		this->PelvisOffset = FVector::ZeroVector;
		return;
	}


	//Step 1 허리의 높이를 지정할 더 낮은 위치의 발의 위치 저장
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
	
	//foot IK활성화 판단
	if (enableFootIK == false)
	{
		currentFootLockAlpha = 0.0f;
		return;
	}

	//footLock값 임시저장
	float footLocakCurveValue = this->GetCurveValue(footLockCurve);


	//2단계: footLocakCurve값이 현재 alpha값 보다 작거나 1인 경우에만 FootLock Alpha를 업데이트합니다. 이렇게 하면 발이 잠긴 위치에서 벗어나거나 새 위치로 잠길 수만 있고 절대 섞이지 않도록 합니다.
	//footLocakCurveValue는 자연스럽게 올라가거나, 자연스럽게 내려가거나, 바로 1이 되는 세가지 값 이동이 있다
	//이 조건식은 currentFootLockAlpha가 바로 1이 되거나 자연스럽게 내려갈수는 있지만 자연스럽게 올라가게 하지는 않습니다. 고로 바로 1이 되거나 1에서 자연스럽게 내려가거나 둘중 하나입니다.
	if (0.99f <= footLocakCurveValue || footLocakCurveValue < currentFootLockAlpha)
	{
		currentFootLockAlpha = footLocakCurveValue;
	}

	//currentFootLockAlpha가 1이 될때 내가 pose로 정해둔 애님의 소켓 transform을 
	//component공간기준 저장합니다.
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

	//현재 회전과 마지막으로 업데이트된 회전 사이의 델타를 사용하여 지면에 고정된 상태를 유지하기 위해 발을 얼마나 회전해야 하는지 찾습니다.
	if (owner->GetCharacterMovement()->IsMovingOnGround() == true)
	{
		rotationDifference = OwnerRotation - owner->GetCharacterMovement()->GetLastUpdateRotation();
	}
	
	//메시 회전을 기준으로 프레임 사이를 이동한 거리를 가져와 지면에 고정된 상태로 유지하기 위해 발이 오프셋되어야 하는 정도를 찾습니다.
	locationDifference = this->GetOwningComponent()->GetComponentRotation().UnrotateVector(OwnerVelocity * GetWorld()->DeltaTimeSeconds);

	//현재 로컬 위치에서 위치 차이를 빼고 회전 차이만큼 회전하여 발이 구성요소 공간에 고정되도록 합니다.
	FVector tempLoc = localLocation - locationDifference;
	tempLoc.RotateAngleAxis(rotationDifference.Yaw, FVector(0, 0, -1.0f));
	localLocation = tempLoc;

	//새 로컬 회전을 얻으려면 현재 로컬 회전에서 회전 차이를 뺍니다.
	localRotation = localRotation - rotationDifference;

	return;
}

