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
	{///90�� �̸��� ���� �������� �����ϴ� ��Ȳ
		maxValue = owner->GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{///90�� �̻� ���̳��� �ٸ� �������� ���ӵǴ� ��Ȳ
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
	//IK���������� Ȯ��
	if (0.0f < GetCurveValue(EnableCurveName) == false)
	{///Foot IK Ȱ��ȭ ���� ������
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
		return;
	}

	/**	|| IKBone ������ Trace�� ��Ƽ� Impact Location�� Normal�� �ӽ����� ||
	 *	
	 *	startTraceLocation	:IK�� ���� Trace�� �� Loc, foot bone�� ��ġ���� Z������ ���� �÷��� ���� �ϴµ�
							 �� ������ ��簰�� ������ ������ �������� ������ ���� �� ��ġ���� ���� �ֱ� ����
	 *	endTraceLocaiton	:IK�� ���� Trace�� ������ ������ Loc
	 *	IKFootFloorLocation :X,Y�� ���� ��ġ�̰� Z�� root bone�� ��ġ�� ���� 
	 *	ImpactPoint			:Trace�� ��� ��ġ
	 *	ImpactNormal		:Trace�� ��� Normal
	 *	TargetRotationOffset
	 *	IKTraceDistance		:�ٸ��� ������� ���θ��� �ʼ� �ִ����� ��
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

			/**	|| foot bone�� ���� Loc,Rot�� �����Ѵ�. ||
			 *	1. CurrentLocationTarget�� ���ϴ� ������ (FVector(0, 0, FootHeight)�� �ƴ� (ImpactNormal*	 	FootHeight)�� �� �ϴ� ������ ���� ���������� �㸮�� �����߽��� �ű���� �ϴ°��̴�.
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
	

	/**	|| ������ ���� �ε巴�� ����ǵ��� �Ѵ�. || */
	CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset, CurrentLocationTarget, deltaSec, 30.0f);
	CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset, TargetRotationOffset, deltaSec, 30.0f);
}


void UCCharacterAnimInstance::SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target)
{
	/**	|| ����� foot IK ���� ����� ���Ͽ� ����ġ(0 ~ 1)�� ���մϴ�. || */
	pelvisAlpha = (GetCurveValue(FName("Enable_FootIK_L")) + GetCurveValue(FName("Enable_FootIK_R"))) / 2.0f;

	if (FMath::IsNearlyZero(pelvisAlpha) == true)
	{///��� ��� ���� �ȵɶ�
		this->PelvisOffset = FVector::ZeroVector;
		return;
	}


	/**	|| �㸮�� ���̸� ������ �� ���� ��ġ�� ���� ��ġ ���� ||
	 *	PelvisTarget : �� ���� ���� ��ġ ����
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

	/**	|| ���ο� ��ġ�� �����̿� ���� �����ӵ��� �ٸ��� �մϴ�. || */
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

	/**	|| FootLock�� �������� Ȯ���մϴ�. ||
	 *	1. FootIK�� Ȱ��ȭ �Ǿ�� Lock�� �����ϱ⶧���� FootIK�� ����ġ�� Ȯ���մϴ�.
	 *	2. ���Ŀ� Foot Lock�� ����ġ�� Ŀ�갪���� Ȯ���Ͽ� �����մϴ�.
	 */

	if (0.0f < GetCurveValue(EnableCurveName) == false)
	{///Foot IK Ȱ��ȭ ���� ������
		currentFootLockAlpha = 0.0f;
		return;
	}
	float footLocakCurveValue = this->GetCurveValue(footLockCurve);


	//2�ܰ�: footLocakCurve���� ���� alpha�� ���� �۰ų� 1�� ��쿡�� FootLock Alpha�� ������Ʈ�մϴ�. �̷��� �ϸ� ���� ��� ��ġ���� ����ų� �� ��ġ�� ��� ���� �ְ� ���� ������ �ʵ��� �մϴ�.
	//footLocakCurveValue�� �ڿ������� �ö󰡰ų�, �������ų� �ٷ� 1�� �Ǵ� ������ �� �̵��� �ִ�
	//�� ���ǽ��� currentFootLockAlph�� �ٷ� 1�� �ǰų� �ڿ������� ���������� ������ �ڿ������� �ö󰡰� ������ �ʽ��ϴ�. ��� �ٷ� 1�� �ǰų� 1���� �ڿ������� �������ų� ���� �ϳ��Դϴ�.
	if (0.99f <= footLocakCurveValue || footLocakCurveValue < currentFootLockAlpha)
	{
		currentFootLockAlpha = footLocakCurveValue;
	}

	//currentFootLockAlpha�� 1�� �ɶ� ���� pose�� ���ص� �ִ��� ���� transform�� 
	//component�������� �����մϴ�.
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

	//���� ȸ���� ���������� ������Ʈ�� ȸ�� ������ ��Ÿ�� ����Ͽ� ���鿡 ������ ���¸� �����ϱ� ���� ���� �󸶳� ȸ���ؾ� �ϴ��� ã���ϴ�.
	if (owner->GetCharacterMovement()->IsMovingOnGround() == true)
	{
		rotationDifference = ownerRotation - owner->GetCharacterMovement()->GetLastUpdateRotation();
	}

	//�޽� ȸ���� �������� ������ ���̸� �̵��� �Ÿ��� ������ ���鿡 ������ ���·� �����ϱ� ���� ���� �����µǾ�� �ϴ� ������ ã���ϴ�.
	locationDifference = this->GetOwningComponent()->GetComponentRotation().UnrotateVector(ownerVelocity * GetWorld()->DeltaTimeSeconds);

	//���� ���� ��ġ���� ��ġ ���̸� ���� ȸ�� ���̸�ŭ ȸ���Ͽ� ���� ������� ������ �����ǵ��� �մϴ�.
	FVector tempLoc = localLocation - locationDifference;
	tempLoc.RotateAngleAxis(rotationDifference.Yaw, FVector(0, 0, -1.0f));
	localLocation = tempLoc;

	//�� ���� ȸ���� �������� ���� ���� ȸ������ ȸ�� ���̸� ���ϴ�.
	localRotation = localRotation - rotationDifference;

	return;
}

