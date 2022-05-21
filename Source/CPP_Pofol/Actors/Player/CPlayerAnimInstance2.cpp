#include "CPlayerAnimInstance2.h"
#include "Actors/Player/CPlayer.h"
#include "Components/CWallRunComponent.h"

void UCPlayerAnimInstance2::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UCPlayerAnimInstance2::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UCPlayerAnimInstance2::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UCPlayerAnimInstance2::UpdateGround_Movement()
{
	Super::UpdateGround_Movement();
}

void UCPlayerAnimInstance2::UpdateGround_Idle()
{
	Super::UpdateGround_Idle();
}

void UCPlayerAnimInstance2::UpdateAir()
{
	Super::UpdateAir();
}

void UCPlayerAnimInstance2::UpdateSubState()
{
	
}

void UCPlayerAnimInstance2::UpdateWallRunValue()
{
	
}