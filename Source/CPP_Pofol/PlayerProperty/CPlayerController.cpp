#include "CPlayerController.h"
#include "CPlayerCameraManager.h"
#include "Interfaces/CIInputFunc.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Util/CLog.h"


ACPlayerController::ACPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//bShowMouseCursor
}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UKismetSystemLibrary::DoesImplementInterface(GetPawn(), UCIInputFunc::StaticClass()) == true)
	{
		myPawn = Cast<ICIInputFunc>(GetPawn());
	}
}

void ACPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	(Cast<ACPlayerCameraManager>(PlayerCameraManager))->OnPosess();
}

void ACPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	InputComponent->BindAxis("MoveForward", this, &ACPlayerController::Move_FB);
	InputComponent->BindAxis("MoveRight", this, &ACPlayerController::Move_RL);
	InputComponent->BindAxis("LookUp", this, &ACPlayerController::Mouse_Vertical);
	InputComponent->BindAxis("LookRight", this, &ACPlayerController::Mouse_Horizontal);

	InputComponent->BindAction("Q", IE_Pressed, this, &ACPlayerController::Press_Q);
	InputComponent->BindAction("E", IE_Pressed, this, &ACPlayerController::Press_E);
	InputComponent->BindAction("R", IE_Pressed, this, &ACPlayerController::Press_R);
	InputComponent->BindAction("1", IE_Pressed, this, &ACPlayerController::Press_1);
	InputComponent->BindAction("2", IE_Pressed, this, &ACPlayerController::Press_2);
	InputComponent->BindAction("3", IE_Pressed, this, &ACPlayerController::Press_3);
	InputComponent->BindAction("4", IE_Pressed, this, &ACPlayerController::Press_4);

	InputComponent->BindAction("Shift_L", IE_Pressed, this, &ACPlayerController::Press_Shitf_L);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACPlayerController::Press_Spacebar);
	InputComponent->BindAction("Ctrl", IE_Pressed, this, &ACPlayerController::Press_Ctrl);
	InputComponent->BindAction("Tab", IE_Pressed, this, &ACPlayerController::Press_Tab);
	InputComponent->BindAction("Mouse_L", IE_Pressed, this, &ACPlayerController::Press_Mouse_L);
	InputComponent->BindAction("Mouse_R", IE_Pressed, this, &ACPlayerController::Press_Mouse_R);
}


void ACPlayerController::Move_FB(float axisValue)
{
	myPawn->Move_FB(axisValue);
}

void ACPlayerController::Move_RL(float axisValue)
{
	myPawn->Move_RL(axisValue);
}

void ACPlayerController::Press_Spacebar()
{
	myPawn->Press_Spacebar();
}

void ACPlayerController::Press_Shitf_L()
{
	myPawn->Press_Shitf_L();
}

void ACPlayerController::Press_Ctrl()
{
	myPawn->Press_Ctrl();
}

void ACPlayerController::Press_Tab()
{
	myPawn->Press_Tab();
}

void ACPlayerController::Press_Q()
{
	myPawn->Press_Q();
}

void ACPlayerController::Press_E()
{
	myPawn->Press_E();
}

void ACPlayerController::Press_R()
{
	myPawn->Press_R();
}

void ACPlayerController::Press_1()
{
	myPawn->Press_1();
}

void ACPlayerController::Press_2()
{
	myPawn->Press_2();
}

void ACPlayerController::Press_3()
{
	myPawn->Press_3();
}

void ACPlayerController::Press_4()
{
	myPawn->Press_4();
}

void ACPlayerController::Mouse_Vertical(float axisValue)
{
	myPawn->Mouse_Vertical(axisValue);
}

void ACPlayerController::Mouse_Horizontal(float axisValue)
{
	myPawn->Mouse_Horizontal(axisValue);
}

void ACPlayerController::Press_Mouse_L()
{
	myPawn->Press_Mouse_L();
}

void ACPlayerController::Press_Mouse_R()
{
	myPawn->Press_Mouse_R();
}
