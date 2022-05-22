#include "CPlayerAnimInstance.h"
#include "Actors/Player/CPlayer.h"
#include "Components/CWallRunComponent.h"

void UCPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ACPlayer * tempPlayer = Cast<ACPlayer>(owner);
	if (tempPlayer != nullptr)
	{
		tempPlayer->GetWallRunComponent()->OnWallJump.BindUFunction(this, FName("OnJump_Animinstance"));
		tempPlayer->GetWallRunComponent()->OnWallStart.BindUFunction(this, FName("OnLand_Animinstance"));
	}
}

void UCPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UCPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
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