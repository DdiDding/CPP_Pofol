#include "CCharacter.h"
#include "Components/CSkillManagerComponent.h"
#include "Components/CWeaponManagerComponent.h"
#include "Components/CParticleManagerComponent.h"
#include "Components/CReactionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Util/CLog.h"


// Sets default values
ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Setting Components
	{
		weaponManagerComponent8 = CreateDefaultSubobject<UCWeaponManagerComponent>(TEXT("WeaponManagerComponent"));
		skillManagerComponent7 = CreateDefaultSubobject<UCSkillManagerComponent>(TEXT("SkillManagerComponent"));
		reactionComponent = CreateDefaultSubobject<UCReactionComponent>(TEXT("ReactionComponent"));
	}

	//Setting MovementComponent
	{
		GetCharacterMovement()->GravityScale = gravityScale;

		//current movement setting 커브저장
		static ConstructorHelpers::FObjectFinder<UCurveVector> MovementCurve(TEXT("CurveVector'/Game/Curves/Locomotion/movementSettings/movementCurve.movementCurve'"));
		currentMovementSettings.movementCurve = MovementCurve.Object;
		static ConstructorHelpers::FObjectFinder<UCurveFloat> RotationRateCurve(TEXT("CurveFloat'/Game/Curves/Locomotion/movementSettings/rotationRateCurve.rotationRateCurve'"));
		currentMovementSettings.rotationRateCurve = RotationRateCurve.Object;
	}

}

void ACCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//Setting characterStructs
	{
		characterStructs = NewObject<UCCharacterStruct>(this, UCCharacterStruct::StaticClass(), TEXT("characterStructs"));
		lastRotationMode = GET_STATE(RotationMode);
	}

	//reactionComponent = NewObject<UCReactionComponent>(this, UCReactionComponent::StaticClass());
	//check(reactionComponent);
	//reactionComponent->RegisterComponent();//동적으로 만들때 사용한다.
	particleManagerComponent = NewObject<UCParticleManagerComponent>(this, UCParticleManagerComponent::StaticClass());
	check(particleManagerComponent);
	particleManagerComponent->RegisterComponent();

	animnInst = GetMesh()->GetAnimInstance();
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCapsuleComponent() != nullptr)
	{
		halfCapusleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	deltaTime = DeltaTime;

	UpdateSubState();
	UpdateEssentialValue();

	if (GET_STATE(SubState) == ESubState::HITTED || GET_STATE(SubState) == ESubState::LAY_DOWN)
	{
		UpdateRagDoll();
	}
	else
	{
		UpdateSetLocation();
		UpdateMovementSetting();
		UpdateRotation();
	}

	UpdateMovementPreviousValue();
}

void ACCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	if (OnJump.IsBound() == true) OnJump.Execute();
	/*SetLastRotationMode(GET_STATE(RotationMode));
	SET_STATE(RotationMode, Velocity);*/
	return;
}

void ACCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);

	if (characterStructs == nullptr) return;
	if (OnLand.IsBound() == true) OnLand.Execute();
	//PutBackLastRotationMode();
	SET_STATE(MainState, Ground);
}

void ACCharacter::Falling()
{
	Super::Falling();

	if (characterStructs == nullptr) return;

	//나중에 공중에서의 Rotation정의하기
	/*SetLastRotationMode(GET_STATE(RotationMode));
	SET_STATE(RotationMode, Velocity);*/
	SET_STATE(MainState, Air);
}


/*********************************************************************************************
	* Update
********************************************************************************************* */

void ACCharacter::UpdateSubState()
{
	if (this == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) return;
	if (characterStructs == nullptr) return;
	if (animnInst == nullptr) return;

	float hittedCurveValue = animnInst->GetCurveValue(FName("Hitted_Curve"));
	//CLog::ScreenLog(hittedCurveValue, 0.f, FColor::Red, "curve value : ");
	if (GET_STATE(SubState) == ESubState::NONE && 0.0f < hittedCurveValue)
	{
		SET_STATE(SubState, Hitted);
		RagDollStart();
		return;
	}
	if (GET_STATE(SubState) == ESubState::HITTED)
	{
		if(hittedCurveValue <= 0.0f)
		{
			SET_STATE(SubState,None);
			RagDollEnd();
			return;
		}
		if ( 1.5f <= hittedCurveValue )
		{///커브값이 2가 되어야 LayDown이 된다.
			SET_STATE(SubState, LayDown);
			return;
		}
	}
	if (GET_STATE(SubState) == ESubState::LAY_DOWN)
	{
		if (hittedCurveValue < 1.5f)
		{
			SET_STATE(SubState, Hitted);
			return;
		}
	}
}

void ACCharacter::UpdateEssentialValue()
{
	currentVelocity = this->GetVelocity();
	acceleration = (currentVelocity - previousVelocity) / deltaTime;
	currentSpeed = currentVelocity.Size();

	if(1.0f < currentSpeed)
	{
		LastVelocityRotation = currentVelocity.Rotation();
	}

	if (0.0f < GetCharacterMovement()->GetCurrentAcceleration().Size())
	{
		LastMovementInputRotation = GetCharacterMovement()->GetCurrentAcceleration().Rotation();
	}

	aimYawRate = FMath::Abs((GetControlRotation().Yaw - previousAimYaw) / deltaTime);
}

void ACCharacter::UpdateGroundRotation()
{
	//상속클래스에서 기타 동작(SubState,...)에 의한 회전을 따로 추가하고싶을경우 이 함수를 오버라이드하세요
	if (AddUpdateGroundedRotation() == true) return;
}


void ACCharacter::UpdateRotation()
{
	//상속클래스에서 기타 동작(SubState,...)에 의한 회전을 따로 추가하고싶을경우 이 함수를 오버라이드하세요
	if (AddUpdateGroundedRotation() == true) return;
	
	if( currentSpeed <= 0.0f )
	{
		return;
	}

	//interpSpeed는 두 모드 모두 동일함
	float interpSpeed = currentMovementSettings.rotationRateCurve->GetFloatValue(GetMappedSpeed());
	FRotator resultRot = FRotator::ZeroRotator;

	if (characterStructs == nullptr) return;
	if (GET_STATE(RotationMode) == ERotationMode::NONE) { return; }
	if (GET_STATE(RotationMode) == ERotationMode::VELOCITY)
	{
		resultRot = SmoothCharacterRotation(LastVelocityRotation.Yaw, 800.0f, CalculateGroundedRotationRate());
	}
	if (GET_STATE(RotationMode) == ERotationMode::CONTROLLER)
	{
		resultRot = SmoothCharacterRotation(GetControlRotation().Yaw, 500.0f, CalculateGroundedRotationRate());
	}


	SetActorRotation(resultRot, ETeleportType::None);

	return;
}


void ACCharacter::UpdateMovementPreviousValue()
{
	previousVelocity = this->GetVelocity();
	previousAimYaw = GetControlRotation().Yaw;
	return;
}


void ACCharacter::UpdateMovementSetting()
{
	if (characterStructs == nullptr) return;
	if (GET_STATE(MainState) == EMainState::GROUND)
	{
		UpdateCurrentMovementSettings();
	}
	if (GET_STATE(MainState) == EMainState::AIR)
	{

	}
	
}

void ACCharacter::UpdateCurrentMovementSettings()
{
	/** || 각 모드에 따라  MaxWalkSpeed와 MaxFlySpeed를 세팅합니다. ||*/
	if(GET_STATE(MoveMode) == EMoveMode::WALKING)
	{
		GetCharacterMovement()->MaxWalkSpeed = currentMovementSettings.animWalkSpeed * multiplySpeed;
		GetCharacterMovement()->MaxFlySpeed = currentMovementSettings.animWalkSpeed * multiplySpeed;
	}
	if(GET_STATE(MoveMode) == EMoveMode::RUNNING)
	{
		GetCharacterMovement()->MaxWalkSpeed = currentMovementSettings.animRunSpeed * multiplySpeed;
		GetCharacterMovement()->MaxFlySpeed = currentMovementSettings.animRunSpeed * multiplySpeed;
	}


	/** || Acc, Dec, Ground Friction을 업데이트 합니다. ||
	 *	curveValue : 커브의 X축은 MaxAcc, Y축은 Dec, Z축은 Ground Friction의 값을 가지고 있습니다.
	 */
	FVector curveValue = currentMovementSettings.movementCurve->GetVectorValue(GetMappedSpeed());

	GetCharacterMovement()->MaxAcceleration = curveValue.X * multiplySpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = curveValue.Y * multiplySpeed;

	if (GET_STATE(MainState) == EMainState::GROUND)
	{
		GetCharacterMovement()->GroundFriction = curveValue.Z;
	}
	else
	{
		GetCharacterMovement()->GroundFriction = 5.0f;
	}

	return;
}


float ACCharacter::GetMappedSpeed()
{
	FVector2D inRange = FVector2D::ZeroVector;
	FVector2D outRange = FVector2D::ZeroVector;

	//걷기상태일경우
	if (currentSpeed <= currentMovementSettings.animWalkSpeed* multiplySpeed)
	{
		inRange = { 0.0f, currentMovementSettings.animWalkSpeed* multiplySpeed };
		outRange = { 0.0f, 1.0f };
	}
	else //if (currentSpeed <= currentMovementSettings.animRunSpeed) 나중에 상태 추가될수도있음
	{
		inRange = { currentMovementSettings.animWalkSpeed* multiplySpeed, currentMovementSettings.animRunSpeed* multiplySpeed };
		outRange = { 1.0f, 2.0f };
	}

	return FMath::GetMappedRangeValueClamped(inRange, outRange, currentSpeed);
}

FRotator ACCharacter::SmoothCharacterRotation(float targetYaw, const float targetInterpSpeed, const float actorInterpSpeed)
{
	FRotator target = FRotator(0.0f, targetYaw, 0.0f);
	targetRotation = FMath::RInterpConstantTo(targetRotation, target, deltaTime, targetInterpSpeed);

	return FMath::RInterpTo(GetActorRotation(), targetRotation, deltaTime, actorInterpSpeed);
}

float ACCharacter::CalculateGroundedRotationRate()
{
	return currentMovementSettings.rotationRateCurve->GetFloatValue(GetMappedSpeed()) *
		UKismetMathLibrary::MapRangeClamped(aimYawRate, 0.0f, 300.0f, 1.0f, 2.0f);
}


void ACCharacter::GetCharacterMovementValue(
	FVector& location, FRotator& rotation, FVector& velocity, float& speed,
	float& animMaxWalkSpeed, float& animMaxRunSpeed, float& multiSpeed, FVector& acc, FVector& DistanceAcc)
{
	location = this->GetActorLocation();
	rotation = this->GetActorRotation();
	velocity = currentVelocity;
	speed = currentSpeed;
	animMaxWalkSpeed = currentMovementSettings.animWalkSpeed * multiplySpeed;
	animMaxRunSpeed = currentMovementSettings.animRunSpeed * multiplySpeed;
	multiSpeed = this->multiplySpeed;
	acc = GetCharacterMovement()->GetCurrentAcceleration();
	DistanceAcc = acceleration;
}


void ACCharacter::UpdateSetLocation()
{
	SetLocation();
}

bool ACCharacter::AddUpdateGroundedRotation()
{
	if (characterStructs == nullptr) return false;
	if (GET_STATE(SubState) == ESubState::ATTACK)
	{
		check(skillManagerComponent7);
		
		FRotator tempRot = FMath::RInterpTo(GetActorRotation(), FRotator(0.0f, skillManagerComponent7->GetAttackYaw(), 0.0f), deltaTime, attackRotInterpSpeed);

		SetActorRotation(tempRot, ETeleportType::None);
		return true;
	}

	return false;
}

void ACCharacter::PutBackLastRotationMode()
{
	if (lastRotationMode == ERotationMode::VELOCITY)
	{
		SET_STATE(RotationMode,Velocity);
	}
	if (lastRotationMode == ERotationMode::CONTROLLER)
	{
		SET_STATE(RotationMode,Controller);
	}
}

void ACCharacter::SetLastRotationMode(ERotationMode mode)
{
	lastRotationMode = mode;
}

/*********************************************************************************************
	* Rag Doll
********************************************************************************************* */

void ACCharacter::RagDollStart()
{
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACCharacter::UpdateRagDoll()
{
	/**	|| 자연스럽게 일어나기 위한 포즈 저장 || */
	GetMesh()->GetAnimInstance()->SavePoseSnapshot(FName("RagdollPose"));


	/**	|| Hitted, LayDown의 상태에 따라 RagDoll weight을 다르게 설정합니다. || */
	if (GET_STATE(SubState) == ESubState::LAY_DOWN)
	{
		if (float hittedCurveValue = animnInst->GetCurveValue(FName("Hitted_Curve")) == 2.0f)
		{
			
			/*if(ragDollWeight < 1.f) ragDollWeight+= 0.07f;
			else if (1.f < ragDollWeight) ragDollWeight = 1.f;*/
			ragDollWeight = 1.f;
			GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, true);
			GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), ragDollWeight, false, true);
			RagDoll_SetCapusleLoc();
			return;
		}
	}

	ragDollWeight = 0.7f;
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("hand_l"), true, true);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("hand_r"), true, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("hand_l"), ragDollWeight, false, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("hand_r"), ragDollWeight, false, true);
	CLog::Log("Update Hitted NOrmal");
	if (GET_STATE(MainState) == EMainState::AIR)
	{
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("calf_l"), true, true);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("calf_r"), true, true);
		CLog::Log("Update Hitted Air");
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("calf_l"), 1.0f, false,true);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("calf_r"), 1.0f, false,true);
	}
	//else if (GET_STATE(MainState) == EMainState::GROUND)
	//{
	//	//CLog::Log("Update Hitted Ground");
	//	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("calf_l"), 0.f, false, true);
	//	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("calf_r"), 0.f, false, true);
	//}
	
	
	return;
}

void ACCharacter::RagDoll_SetCapusleLoc()
{
	FVector TargetRagdolLoc = GetMesh()->GetSocketLocation(FName("pelvis"));
	FRotator TargetRagdolRot = GetMesh()->GetSocketRotation(FName("pelvis"));

	bIsRagdolFaceUp = TargetRagdolRot.Roll < 0.0f;
	if (bIsRagdolFaceUp == true)
	{///윗면을 향할때
		TargetRagdolRot.Yaw = TargetRagdolRot.Yaw - 180.0f;
	}
	TargetRagdolRot.Roll = 0.0f;
	TargetRagdolRot.Pitch = 0.0f;


	FHitResult hitResult;
	float capsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector endLoc = FVector(TargetRagdolLoc.X, TargetRagdolLoc.Y,
		TargetRagdolLoc.Z - capsuleHalfHeight);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, TargetRagdolLoc, endLoc,
		ECollisionChannel::ECC_Visibility) == false)
	{
		bRagdollOnGround = false;
		SetActorLocationAndRotation(TargetRagdolLoc, TargetRagdolRot);
		return;
	}

	FVector setLoc;
	setLoc.X = TargetRagdolLoc.X;
	setLoc.Y = TargetRagdolLoc.Y;
	setLoc.Z = TargetRagdolLoc.Z + 2.0f +
		capsuleHalfHeight - FMath::Abs(hitResult.ImpactPoint.Z - TargetRagdolLoc.Z);
	
	bRagdollOnGround = true;
	float interpSpeed = 15.0f;
	
	SetActorLocationAndRotation(setLoc, TargetRagdolRot);
}

void ACCharacter::RagDollEnd()
{
	//CLog::Log("End Ragdoll");
	RagDollStartTimer();

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	//GetMesh()->GetAnimInstance()->Montage_Play(GetGetUpAnimaMontage(bIsRagdolFaceUp));
}

void ACCharacter::RagDollStartTimer()
{
	if (GetWorldTimerManager().IsTimerActive(endRagdollHandle) == true)
	{///실행되고있었다면 취소
		GetWorldTimerManager().ClearTimer(endRagdollHandle);
	}
	GetWorldTimerManager().SetTimer(endRagdollHandle, this, &ACCharacter::RagDollEndTimer, GetWorld()->GetDeltaSeconds(), true);
}

//TODO:나중에 Lagdoll에서 일어날때 맞는경우는 일어나는 애니메이션실행해야하니까 다른 처리해야함
void ACCharacter::RagDollEndTimer()
{
	ragDollWeight -= 0.03f;
	if (ragDollWeight <= 0.0f)
	{///레그돌 해제 시키기
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetWorldTimerManager().ClearTimer(endRagdollHandle);
		return;
	}
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("spine_03"), ragDollWeight, false, true);
}

void ACCharacter::GetCharacterState(
	EMainState& mainState_out, ESubState& subState_out, EMoveMode& movemode_out)
{
	if (characterStructs == nullptr) return;
	mainState_out = characterStructs->GetMainState();
	subState_out = characterStructs->GetSubState();
	movemode_out = characterStructs->GetMoveMode();
	return;
}



//if (GET_STATE(MainState) == EMainState::GROUND)
//{
//	float charHalfZ = GetActorLocation().Z;
//	float groundZ = GetActorLocation().Z - halfCapusleHeight;
//	float determinLayDownZ = FMath::Lerp(charHalfZ, groundZ, ratioDeterminLayDownZ);
//
//	//if (GetMesh()->GetSocketLocation(FName("pelvis")).Z < determinLayDownZ)
//	if (120.f < GetMesh()->GetSocketLocation(FName("pelvis")).Z)
//	{
//		SET_STATE(SubState, LayDown);
//		return;
//	}
//}


//if (GET_STATE(SubState) == ESubState::LAY_DOWN)
//{
//	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, true);
//	///**	|| 너무 부자연스럽게 꺾이지 않도록 속도에따라 단단하게 관절을 구성합니다 || */
//	LastRagDollVelocity = GetMesh()->GetPhysicsLinearVelocity();
//
//	if (LastRagDollVelocity.Size() < 1.5f)
//	{
//		bCanGetUp = true;
//	}
//	else
//	{
//		bCanGetUp = false;
//	}
//
//	float inSpring = UKismetMathLibrary::MapRangeClamped(
//		LastRagDollVelocity.Size(),
//		0.0f, 1000.0f,
//		0.0f, 25000.0f
//	);
//	GetMesh()->SetAllMotorsAngularDriveParams(inSpring, 0.0f, 0.0f);
//
//	///**	|| 캡슐을 메시에 붙입니다. || */
//	RagDoll_SetCapusleLoc();
//	return;
//}