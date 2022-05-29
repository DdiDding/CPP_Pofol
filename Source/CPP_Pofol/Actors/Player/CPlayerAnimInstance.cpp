#include "CPlayerAnimInstance.h"
#include "Actors/Player/CPlayer.h"
#include "Components/CWallRunComponent.h"
#include "Util/CLog.h"

void UCPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ACPlayer * tempPlayer = Cast<ACPlayer>(owner);
	if (tempPlayer != nullptr)
	{
		tempPlayer->GetWallRunComponent()->OnWallJump.BindUFunction(this, FName("OnJump_Animinstance"));
		tempPlayer->GetWallRunComponent()->OnWallStart.BindUFunction(this, FName("OnLand_Animinstance"));
		wrc = tempPlayer->GetWallRunComponent();
	}
}

void UCPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UCPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (owner == nullptr || owner->GetCharacterStruct() == nullptr) return;

	if (owner->GetSubState() == ESubState::WALLRUN)
	{
		if (wrc == nullptr) return;
		wallRunRoll = wrc->GetMovingDegree();
		CLog::Log("UpdateWallRun");
	}
}

void UCPlayerAnimInstance::UpdateGround_Movement()
{
	Super::UpdateGround_Movement();
}

void UCPlayerAnimInstance::UpdateGround_Idle()
{
	Super::UpdateGround_Idle();
}

void UCPlayerAnimInstance::UpdateAir()
{
	Super::UpdateAir();
}

void UCPlayerAnimInstance::UpdateSubState()
{
	
}

void UCPlayerAnimInstance::UpdateWallRunValue()
{
	
}