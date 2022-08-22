#include "CPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CWallRunComponent.h"
#include "Components/CSkillManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "CGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Util/CLog.h"

ACPlayer::ACPlayer()
{
	//Property Setting
	{
		PrimaryActorTick.bCanEverTick = true;
		bUseControllerRotationYaw = false;
	}

	//CharacterMovement Setting
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
		GetCharacterMovement()->GravityScale = 1.3f;
		this->bUseControllerRotationYaw = false;

		currentMovementSettings.animWalkSpeed = 304.0248f;
		currentMovementSettings.animRunSpeed = 717.0f;
		multiplySpeed = 1.8f;
	}

	//Setting AnimInstance
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		//AnimInstance 저장
		/*static ConstructorHelpers::FClassFinder<UAnimInstance>PlayerABP(TEXT("UAnimInstance'/Game/Actors/Player/ABP_PlayerABP.ABP_PlayerABP_C'"));
		GetMesh()->SetAnimInstanceClass(PlayerABP.Class);*/
	}

	//SkeletalMesh Init
	{
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

		//스켈레탈 가져오기
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CyberSamurai(TEXT("SkeletalMesh'/Game/Resources/Skeletals/CyberpunkSamurai/Meshes/SK_CyberpunkSamurai_WithHelmet.SK_CyberpunkSamurai_WithHelmet'"));

		//애셋 적용
		GetMesh()->SetSkeletalMesh(SK_CyberSamurai.Object);
	}

	//SubMovementComponent
	{
		wallRunComponent = CreateDefaultSubobject<UCWallRunComponent>(TEXT("WallRunComponent"));
		//wallRunComponent의 Tick이 끝나야 CPlayer의 Tick을 실행한다는 종속성 설정
		AddTickPrerequisiteComponent(wallRunComponent);
	}
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (characterStructs == nullptr) return;
	SET_STATE(RotationMode, Controller);
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	allTime += DeltaTime;

	UpdateProperty();
	if (bOnDebugMode == true)Debug();
}

////////////////////////////////////////////////////////////////////////////////
//Update
void ACPlayer::UpdateProperty()
{
	controllerForVec = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);
	float arrowLength = 800.0f;
	float arrowSize = 2000.0f;
	float arrowThick = 10.0f;
}

////////////////////////////////////////////////////////////////////////////////
//Input Key function
void ACPlayer::Move_FB(float axisValue)
{
	if (characterStructs == nullptr) return;
	if (GET_STATE(SubState) == ESubState::ATTACK) return;

	virtualAcceleration.X = axisValue;
	FVector Direction;

	if (GET_STATE(SubState) == ESubState::NONE)
	{
		Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		//Controller->GetActorForwardVector.Rotation();
	}
	if (GET_STATE(SubState) == ESubState::WALLRUN)
	{
		Direction = wallRunComponent->GetFinalDir(true, axisValue);
		axisValue = FMath::Abs(axisValue);
	}

	AddMovementInput(Direction, axisValue);
}

void ACPlayer::Move_RL(float axisValue)
{
	if (characterStructs == nullptr) return;
	if (GET_STATE(SubState) == ESubState::ATTACK) return;

	virtualAcceleration.Y = axisValue;
	FVector Direction;

	if (GET_STATE(SubState) == ESubState::NONE)
	{
		Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	}
	if (GET_STATE(SubState) == ESubState::WALLRUN)
	{
		Direction = wallRunComponent->GetFinalDir(false, axisValue);
		axisValue = FMath::Abs(axisValue);
	}

	AddMovementInput(Direction, axisValue);
}

void ACPlayer::Press_Spacebar( )
{
	if (characterStructs != nullptr) { SET_STATE(MainState,Air); }
	if (wallRunComponent != nullptr) {	if (wallRunComponent->StartSubState() == true)	return;	}

	Jump();
}

void ACPlayer::Press_Shitf_L( )
{
	GetCharacterMovement()->Velocity = currentVelocity * 2.0f;
}

void ACPlayer::Press_Ctrl( )
{
	//True면 Running 모드입니다.
	static bool bToggleRunning = true;

	bToggleRunning = !bToggleRunning;
	if (bToggleRunning == true)
	{
		SET_STATE(MoveMode, Running);
	}
	else
	{
		SET_STATE(MoveMode, Walking);
	}
}

void ACPlayer::Press_Tab( )
{
	check(characterStructs);

	SetLastRotationMode(GET_STATE(RotationMode));
	if (GET_STATE(RotationMode) == ERotationMode::VELOCITY)
	{
		SET_STATE(RotationMode, Controller);
	}
	else if (GET_STATE(RotationMode) == ERotationMode::CONTROLLER)
	{
		SET_STATE(RotationMode, Velocity);
	}
}

void ACPlayer::Mouse_Vertical(float axisValue)
{
	AddControllerPitchInput(axisValue);
}

void ACPlayer::Mouse_Horizontal(float axisValue)
{
	AddControllerYawInput(axisValue);
}

void ACPlayer::Press_Mouse_L()
{
	skillManagerComponent7->LaunchSkill("Mouse_L");
}

void ACPlayer::Press_Mouse_R()
{
	skillManagerComponent7->LaunchSkill("Mouse_R");
}

void ACPlayer::Press_Q()
{
	skillManagerComponent7->LaunchSkill("Q");
}

void ACPlayer::Press_E()
{
	skillManagerComponent7->LaunchSkill("E");
}

void ACPlayer::Press_R()
{
	skillManagerComponent7->LaunchSkill("R");
}

void ACPlayer::Press_1()
{
	skillManagerComponent7->LaunchSkill("1");
}

void ACPlayer::Press_2()
{
	skillManagerComponent7->LaunchSkill("2");
}

void ACPlayer::Press_3()
{
	skillManagerComponent7->LaunchSkill("3");
}

void ACPlayer::Press_4()
{
	skillManagerComponent7->LaunchSkill("4");
}

////////////////////////////////////////////////////////////////////////////////
//Location & Rotation
void ACPlayer::SetLocation()
{
	if(characterStructs == nullptr) return;
	if (GET_STATE(SubState) == ESubState::WALLRUN)
	{
		wallRunComponent->UpdateLocation();
	}
}

bool ACPlayer::AddUpdateGroundedRotation()
{
	if (Super::AddUpdateGroundedRotation() == true) return true;
	//TODO:보간넣기

	if (GET_STATE(SubState) == ESubState::WALLRUN)
	{
		wallRunComponent->UpdateRotation();
		return true;
	}

	return false;
}

//Debug
void ACPlayer::Debug() {

	//arrow debug property
	float arrowLength = 400.0f;
	float arrowSize = 2000.0f;
	float arrowThick = 10.0f;
	
	//Debug States
	{
		//(int)&characterStructs
		if (IsValid(characterStructs) == true)
		{
			CLog::ScreenLog(L"@@@ Struct Valid @@@", 0.0f, FColor::Magenta);
		}
		else 
		{
			CLog::ScreenLog(L"@@@ Struct Not Valid @@@", 0.0f, FColor::Magenta);
		}

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString::Printf(
			TEXT("//MainState : %s //SubState : %s"),
			*UEnum::GetDisplayValueAsText(GET_STATE(MainState)).ToString(),
			*UEnum::GetDisplayValueAsText(GET_STATE(SubState)).ToString()));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString::Printf(
			TEXT("//MoveMode : %s //RotationMode : %s"),
			*UEnum::GetDisplayValueAsText(GET_STATE(MoveMode)).ToString(),
			*UEnum::GetDisplayValueAsText(GET_STATE(RotationMode)).ToString()));


		CLog::ScreenLog((int)characterStructs->GetMainState(),0.0f, FColor::Orange,L"MainState to int : ");
		CLog::ScreenLog((int)characterStructs->GetSubState(),0.0f, FColor::Orange,L"SubState to int : ");
		CLog::ScreenLog((int)characterStructs->GetMoveMode(), 0.0f, FColor::Orange, L"MoveMode to int : ");
		CLog::ScreenLog((int)characterStructs->GetRotationMode(), 0.0f, FColor::Orange, L"RotationMode to int : ");
		CLog::ScreenLog(multiplySpeed, 0.0f, FColor::Red, L"Current speed : ");
		
	}

	//Velocity debug - Yellow
	{
		DrawDebugDirectionalArrow(GetWorld(), this->GetActorLocation(),
			(this->GetActorLocation() + currentVelocity),
			arrowSize, FColor::Yellow, false, -1.0f, 0, arrowThick);
	}

	//Input acceletion debug (축에 상관없이 진짜 방향키가 가르키는 곳) - Purple
	{
		FRotator controllerRot = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();
		controllerRot.Pitch = 0.0f;
		controllerRot.Roll = 0.0f;

		DrawDebugDirectionalArrow(GetWorld(), this->GetActorLocation(),
			(this->GetActorLocation() + (controllerRot.RotateVector(virtualAcceleration) * arrowLength)),
			arrowSize, FColor::Purple, false, -1.0f, 0, arrowThick);
	}

	//controller forward debug - Blue
	{
		FRotator tempController = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();
		FVector tempControllerForward = FQuat(tempController).GetForwardVector();

		DrawDebugDirectionalArrow(GetWorld(), this->GetActorLocation(),
			(this->GetActorLocation() + (tempControllerForward * arrowLength)),
			arrowSize, FColor::Blue, false, -1.0f, 0, arrowThick);
	}

	//actor forward debug
	{
		DrawDebugDirectionalArrow(GetWorld(), this->GetActorLocation(),
			(this->GetActorLocation() + (this->GetActorForwardVector() * arrowLength)),
			arrowSize, FColor::Black, false, -1.0f, 0, arrowThick);

		DrawDebugDirectionalArrow(GetWorld(), this->GetActorLocation(),
			(this->GetActorLocation() + (this->GetActorRightVector() * arrowLength)),
			arrowSize, FColor::White, false, -1.0f, 0, arrowThick);
	}

	CLog::ScreenLog(Controller->GetControlRotation(), 0.0f, FColor::Red, L"ControlRotation : ");
	CLog::ScreenLog(GetActorRotation(), 0.0f, FColor::Blue, L"Rotation : ");
	CLog::ScreenLog(GetActorLocation(), 0.0f, FColor::Blue, L"Location : ");

	CLog::ScreenLog(allTime, 0.0f, FColor::Blue, L"Time : ");

}


void ACPlayer::TestPublic()
{
	bool result = Test();

	if (result == true)
	{
		Test_True();
	}
	if (result == false)
	{
		Test_False();
	}
}

bool ACPlayer::Test()
{
	bool result;
	static int num = 0;
	static float worldDeleation = 0.0f;
	float runtime = UGameplayStatics::GetTimeSeconds(GetWorld());

	//0.5초 이내로 눌렀는지 계산
	if (1 <= num && (runtime - worldDeleation) < 0.5f)
	{
		///빠르게 두번 눌렀을 때
		result = true;
		num = 0;
	}
	else
	{
		///나머지
		result = false;
		num++;
	}

	worldDeleation = UGameplayStatics::GetTimeSeconds(GetWorld());

	return result;
}

void ACPlayer::Test_True()
{
	CLog::Log("True");
}

void ACPlayer::Test_False()
{
	CLog::Log("False");
}