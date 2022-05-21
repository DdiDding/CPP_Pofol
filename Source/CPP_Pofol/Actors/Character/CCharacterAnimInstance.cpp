#include "Actors/Character/CCharacterAnimInstance.h"
#include "Actors/Character/CCharacter.h"
#include "Components/CReactionComponent.h"

#include "Util/CLog.h"


void UCCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	owner = Cast<ACCharacter>(this->TryGetPawnOwner());
}


void UCCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (owner != nullptr)
	{
		owner->OnJump.BindUFunction(this, FName("OnJump_Animinstance"));
		owner->OnLand.BindUFunction(this, FName("OnLand_Animinstance"));
		owner->GetReactionComponent()->OnHitted.BindUObject(this, &UCCharacterAnimInstance::OnHitted_Animinstance);
	}
}


void UCCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (owner == nullptr)
	{
		owner = Cast<ACCharacter>(this->TryGetPawnOwner());
		return;
	}
	deltaTime = DeltaSeconds;

	/** || Update every Tick || */
	{
		UpdateOwnerValue();
		UpdateDefaultValue();
		UpdateFootIK();
	}

	/** || Update Main State || */
	{
		if (ownerMainState == EMainState::GROUND)
		{
			if (0.0f < ownerSpeed)
			{
				UpdateGround_Movement();
			}
			else
			{
				UpdateGround_Idle();
			}
		}
		if (ownerMainState == EMainState::AIR)
		{
			UpdateAir();
		}
	}
	
	/** || Update Sub State || */
	{
		UpdateSubState();
	}
}

void UCCharacterAnimInstance::OnJump_Animinstance()
{
	bIsJump = true;
	return;
}

void UCCharacterAnimInstance::OnLand_Animinstance()
{
	bIsJump = false;
	return;
}

void UCCharacterAnimInstance::OnHitted_Animinstance(EReactionType data)
{
	reactionType = data;
	bIsHitted = true;
	OnHitted_Animinstance_Internal();
	return;
}


/*********************************************************************************************
	* Update Function
********************************************************************************************* */

void UCCharacterAnimInstance::UpdateOwnerValue()
{
	owner->GetCharacterMovementValue(
		ownerLocation, ownerRotation, ownerVelocity, ownerSpeed, ownerAnimatedWalkSpeed, ownerAnimatedRunSpeed, ownerMulSpeed, ownerAcceleration, ownerDistanceAcc);

	owner->GetCharacterState(ownerMainState, ownerSubState, ownerMoveMode);
}

void UCCharacterAnimInstance::UpdateDefaultValue()
{
	//Update montagePlayTimeInRatio
	if (IsValid(GetCurrentActiveMontage()))
	{
		montagePlayTimeInRatio = Montage_GetPosition(GetCurrentActiveMontage()) / GetCurrentActiveMontage()->GetPlayLength();
	}
}

void UCCharacterAnimInstance::UpdateGround_Movement()
{
	CalculateVelocityBlend();
	CalculateRelativeAccelerationAmount();
	CalculateWalkRunBlend();
	CalculateStrideBlend();
	CalculateStandingPlayRate();
}

void UCCharacterAnimInstance::UpdateGround_Idle()
{

}

void UCCharacterAnimInstance::UpdateAir()
{

}

void UCCharacterAnimInstance::UpdateSubState()
{
	return;
}

void UCCharacterAnimInstance::UpdateFootIK()
{
	return;
}

/*********************************************************************************************
	* Default Function
********************************************************************************************* */


/*********************************************************************************************
	* Update Movement Ground
********************************************************************************************* */
void UCCharacterAnimInstance::CalculateVelocityBlend()
{
	FVelocityBlend2 tempVelocityBlend;
	FVector tempVelocity = ownerVelocity;
	tempVelocity = ownerRotation.UnrotateVector(tempVelocity);
	FVector tempAbs = tempVelocity.GetAbs();
	float totalMoveAmount = tempAbs.X + tempAbs.Y;
	

	tempVelocityBlend.F = FMath::Clamp(tempVelocity.X / totalMoveAmount, 0.0f, 1.0f);
	tempVelocityBlend.B = FMath::Abs(FMath::Clamp(tempVelocity.X / totalMoveAmount, -1.0f, 0.0f));
	tempVelocityBlend.R = FMath::Clamp(tempVelocity.Y / totalMoveAmount, 0.0f, 1.0f);
	tempVelocityBlend.L	= FMath::Abs(FMath::Clamp(tempVelocity.Y / totalMoveAmount, -1.0f, 0.0f));


	velocityBlend.F =FMath::FInterpTo(velocityBlend.F, tempVelocityBlend.F, deltaTime, velBlendInterpSpeed);
	velocityBlend.B = FMath::FInterpTo(velocityBlend.B, tempVelocityBlend.B, deltaTime, velBlendInterpSpeed);
	velocityBlend.L = FMath::FInterpTo(velocityBlend.L, tempVelocityBlend.L, deltaTime, velBlendInterpSpeed);
	velocityBlend.R = FMath::FInterpTo(velocityBlend.R, tempVelocityBlend.R, deltaTime, velBlendInterpSpeed);
}

void UCCharacterAnimInstance::CalculateRelativeAccelerationAmount()
{
	float maxValue;
	if (0.0f < FVector::DotProduct(ownerDistanceAcc, ownerVelocity))
	{///90도 미만의 같은 방향으로 가속하는 상황
		maxValue = owner->GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{///90도 이상 차이나는 다른 방향으로 감속되는 상황
		maxValue = owner->GetCharacterMovement()->GetMaxBrakingDeceleration();
	}

	relativeAccelerationAmount = owner->GetActorRotation().UnrotateVector(
		ownerDistanceAcc.GetClampedToMaxSize(maxValue) / maxValue
	);

	leanAmount.FB = FMath::FInterpTo(leanAmount.FB, relativeAccelerationAmount.X, deltaTime, leanAmountInterpSpeed);
	leanAmount.LR = FMath::FInterpTo(leanAmount.LR, relativeAccelerationAmount.Y, deltaTime, leanAmountInterpSpeed);
}

void UCCharacterAnimInstance::CalculateWalkRunBlend()
{
	if (ownerMoveMode == EMoveMode::WALKING)
	{
		walkRunBlend = 0.0f;
		return;
	}
	if (ownerMoveMode == EMoveMode::RUNNING)
	{
		walkRunBlend = ownerSpeed / ownerAnimatedRunSpeed;
		return;
	}
}

void UCCharacterAnimInstance::CalculateStrideBlend()
{
	if (strideBlendNormal == nullptr) return;
	strideBlend = strideBlendNormal->GetFloatValue(ownerSpeed / ownerAnimatedWalkSpeed);

	/*strideBlend = FMath::Lerp(
		strideBlendWalk->GetFloatValue(ownerSpeed),
		strideBlendRun->GetFloatValue(ownerSpeed),
		GetAnimcurveClamped(FName("Weight_Gait"), -1.0f, 0.0f, 1.0f)
	);*/
}
void UCCharacterAnimInstance::CalculateStandingPlayRate()
{
	standingPlayRate = FMath::Lerp(
		ownerSpeed / ownerAnimatedWalkSpeed,
		ownerSpeed / ownerAnimatedRunSpeed,
		GetAnimcurveClamped(FName("Weight_Gait"), -1.0f, 0.0f, 1.0f)
	);
	
	standingPlayRate /= strideBlend;
	standingPlayRate *= ownerMulSpeed;

	standingPlayRate /= GetOwningComponent()->GetComponentScale().Z;
}
float UCCharacterAnimInstance::GetAnimcurveClamped(FName curveName, float bias, float clampMin, float clampMax)
{
	return FMath::Clamp(GetCurveValue(curveName) + bias, clampMin, clampMax);
}

/*********************************************************************************************
	* Foot IK
********************************************************************************************* */

void UCCharacterAnimInstance::SetFootOffset(FName EnableCurveName, FName IKFootBone, FName RootBone,
	FVector & CurrentLocationTarget, FVector & CurrentLocationOffset, FRotator & CurrentRotationOffset)
{
	//IK적용중인지 확인
	if (0.0f < GetCurveValue(EnableCurveName) == false)
	{///Foot IK 활성화 되지 않으면
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
		return;
	}

	/**	|| IKBone 밑으로 Trace를 쏘아서 Impact Location과 Normal을 임시저장 ||
	 *	
	 *	startTraceLocation	:IK를 위한 Trace를 쏠 Loc, foot bone의 위치에서 Z축으로 조금 올려서 시작 하는데
							 그 이유는 경사같은 곳에서 옆으로 서있으면 한쪽은 원래 발 위치보다 높게 있기 때문
	 *	endTraceLocaiton	:IK를 위한 Trace를 어디까지 쏠지의 Loc
	 *	IKFootFloorLocation :X,Y는 발의 위치이고 Z는 root bone의 위치를 저장 
	 *	ImpactPoint			:Trace의 결과 위치
	 *	ImpactNormal		:Trace의 결과 Normal
	 *	TargetRotationOffset
	 *	IKTraceDistance		:다리를 어느정도 구부리고 필수 있는지의 값
	 */
	USkeletalMeshComponent * ownerSkeletal = GetOwningComponent();
	FVector startTraceLocation = FVector::ZeroVector;
	FVector endTraceLocaiton = FVector::ZeroVector;
	FVector IKFootFloorLocation = FVector::ZeroVector;
	FVector ImpactPoint = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;
	FRotator TargetRotationOffset = FRotator::ZeroRotator;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(TEXT("ikTrace"), false, owner);

	float IKTraceDistance = 50.0f;
	float deltaSec = GetWorld()->DeltaTimeSeconds;

	IKFootFloorLocation = ownerSkeletal->GetSocketLocation(IKFootBone);
	IKFootFloorLocation.Z = ownerSkeletal->GetSocketLocation(RootBone).Z;

	startTraceLocation = IKFootFloorLocation;
	startTraceLocation.Z += IKTraceDistance;

	endTraceLocaiton = IKFootFloorLocation;
	endTraceLocaiton.Z -= IKTraceDistance;

	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		startTraceLocation,
		endTraceLocaiton,
		ECollisionChannel::ECC_Visibility,
		TraceParams) == true)
	{
		//if (owner->GetCharacterMovement()->IsWalkable(HitResult) == true)
		{
			ImpactPoint = HitResult.ImpactPoint;
			ImpactNormal = HitResult.ImpactNormal;

			/**	|| foot bone에 최종 Loc,Rot을 저장한다. ||
			 *	1. CurrentLocationTarget을 구하는 과정에 (FVector(0, 0, FootHeight)가 아닌 (ImpactNormal*	 	FootHeight)을 더 하는 과정은 발이 낮은쪽으로 허리의 무게중심을 옮기려고 하는것이다.
			 */
			CurrentLocationTarget = (ImpactPoint + (ImpactNormal * FootHeight)) - (IKFootFloorLocation + (FVector(0, 0, FootHeight)));

			TargetRotationOffset = FRotator(
				FMath::RadiansToDegrees(atan2(ImpactNormal.X, ImpactNormal.Z) * -1.0f),
				0.0f,
				FMath::RadiansToDegrees(atan2(ImpactNormal.Y, ImpactNormal.Z)));
		}
	}
	else
	{
		CurrentLocationTarget = FVector::ZeroVector;
		TargetRotationOffset = FRotator::ZeroRotator;
	}
	DEBUG_CCHAR_ANIM
	{
		FColor color;
		if (HitResult.bBlockingHit == true)
		{
			color = FColor::Green;
			DrawDebugSolidBox(GetWorld(), HitResult.ImpactPoint, FVector(10.f), FColor::Blue, false, -1, 0);
		}
		else 
		{
			color = FColor::Red;
		}
		DrawDebugLine(GetWorld(), startTraceLocation, endTraceLocaiton, color, false, -1.0f, 0, 5);
	}
	

	/**	|| 보간을 통해 부드럽게 적용되도록 한다. || */
	CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset, CurrentLocationTarget, deltaSec, 30.0f);
	CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset, TargetRotationOffset, deltaSec, 30.0f);
}


void UCCharacterAnimInstance::SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target)
{
	/**	|| 양발의 foot IK 값의 평균을 구하여 적용치(0 ~ 1)을 구합니다. || */
	pelvisAlpha = (GetCurveValue(FName("Enable_FootIK_L")) + GetCurveValue(FName("Enable_FootIK_R"))) / 2.0f;

	if (FMath::IsNearlyZero(pelvisAlpha) == true)
	{///양발 모두 적용 안될때
		this->PelvisOffset = FVector::ZeroVector;
		return;
	}


	/**	|| 허리의 높이를 지정할 더 낮은 위치의 발의 위치 저장 ||
	 *	PelvisTarget : 더 낮은 발의 위치 저장
	 */
	FVector PelvisTarget = FVector::ZeroVector;

	if (FootOffset_L_Target.Z < FootOffset_R_Target.Z)
	{
		PelvisTarget = FootOffset_L_Target;
	}
	else 
	{
		PelvisTarget = FootOffset_R_Target;
	}

	/**	|| 새로운 위치의 높낮이에 따라 보간속도를 다르게 합니다. || */
	float interpSpeed = 0.0f;
	if (PelvisOffset.Z < PelvisTarget.Z)
	{
		interpSpeed = 10.0f;
	}
	else
	{
		interpSpeed = 15.0f;
	}
	this->PelvisOffset = FMath::VInterpTo(PelvisOffset, PelvisTarget, GetWorld()->DeltaTimeSeconds, interpSpeed);
}


void UCCharacterAnimInstance::SetFootLocking(FName EnableCurveName, FName footLockCurve,
	FName ikFootBone, float& currentFootLockAlpha, FVector& currentFootLockLocation, 
	FRotator& currentFootLockRotation)
{
	if (owner == nullptr) return;

	/**	|| FootLock이 가능한지 확인합니다. ||
	 *	1. FootIK가 활성화 되어야 Lock도 가능하기때문에 FootIK의 적용치를 확인합니다.
	 *	2. 그후에 Foot Lock의 적용치도 커브값으로 확인하여 저장합니다.
	 */

	if (0.0f < GetCurveValue(EnableCurveName) == false)
	{///Foot IK 활성화 되지 않으면
		currentFootLockAlpha = 0.0f;
		return;
	}
	float footLocakCurveValue = this->GetCurveValue(footLockCurve);


	//2단계: footLocakCurve값이 현재 alpha값 보다 작거나 1인 경우에만 FootLock Alpha를 업데이트합니다. 이렇게 하면 발이 잠긴 위치에서 벗어나거나 새 위치로 잠길 수만 있고 절대 섞이지 않도록 합니다.
	//footLocakCurveValue는 자연스럽게 올라가거나, 내려가거나 바로 1이 되는 세가지 값 이동이 있다
	//이 조건식은 currentFootLockAlph가 바로 1이 되거나 자연스럽게 내려갈수는 있지만 자연스럽게 올라가게 하지는 않습니다. 고로 바로 1이 되거나 1에서 자연스럽게 내려가거나 둘중 하나입니다.
	if (0.99f <= footLocakCurveValue || footLocakCurveValue < currentFootLockAlpha)
	{
		currentFootLockAlpha = footLocakCurveValue;
	}

	//currentFootLockAlpha가 1이 될때 내가 pose로 정해둔 애님의 소켓 transform을 
	//component공간기준 저장합니다.
	if (0.99f < currentFootLockAlpha)
	{
		FTransform tempTransform = this->GetOwningComponent()->GetSocketTransform(ikFootBone, ERelativeTransformSpace::RTS_Component);
		currentFootLockLocation = tempTransform.GetLocation();
		currentFootLockRotation = tempTransform.GetRotation().Rotator();
	}

	if (0.0f < currentFootLockAlpha)
	{
		SetFootLockOffset(currentFootLockLocation, currentFootLockRotation);
	}

	return;
}

void UCCharacterAnimInstance::SetFootLockOffset(FVector& localLocation, FRotator& localRotation)
{

	FVector locationDifference;
	FRotator rotationDifference;

	//현재 회전과 마지막으로 업데이트된 회전 사이의 델타를 사용하여 지면에 고정된 상태를 유지하기 위해 발을 얼마나 회전해야 하는지 찾습니다.
	if (owner->GetCharacterMovement()->IsMovingOnGround() == true)
	{
		rotationDifference = ownerRotation - owner->GetCharacterMovement()->GetLastUpdateRotation();
	}

	//메시 회전을 기준으로 프레임 사이를 이동한 거리를 가져와 지면에 고정된 상태로 유지하기 위해 발이 오프셋되어야 하는 정도를 찾습니다.
	locationDifference = this->GetOwningComponent()->GetComponentRotation().UnrotateVector(ownerVelocity * GetWorld()->DeltaTimeSeconds);

	//현재 로컬 위치에서 위치 차이를 빼고 회전 차이만큼 회전하여 발이 구성요소 공간에 고정되도록 합니다.
	FVector tempLoc = localLocation - locationDifference;
	tempLoc.RotateAngleAxis(rotationDifference.Yaw, FVector(0, 0, -1.0f));
	localLocation = tempLoc;

	//새 로컬 회전을 얻으려면 현재 로컬 회전에서 회전 차이를 뺍니다.
	localRotation = localRotation - rotationDifference;

	return;
}

