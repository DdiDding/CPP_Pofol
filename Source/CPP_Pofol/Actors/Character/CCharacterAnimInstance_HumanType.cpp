#include "CCharacterAnimInstance_HumanType.h"
#include "Util/CLog.h"


void UCCharacterAnimInstance_HumanType::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UCCharacterAnimInstance_HumanType::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UCCharacterAnimInstance_HumanType::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}

void UCCharacterAnimInstance_HumanType::UpdateGround_Movement()
{
	Super::UpdateGround_Movement();
}

void UCCharacterAnimInstance_HumanType::UpdateGround_Idle()
{
	Super::UpdateGround_Idle();
}

void UCCharacterAnimInstance_HumanType::UpdateAir()
{
	Super::UpdateAir();
}

void UCCharacterAnimInstance_HumanType::UpdateSubState()
{
	Super::UpdateSubState();
}

void UCCharacterAnimInstance_HumanType::UpdateFootIK()
{
	/** Update Foot Locking */
	SetFootLocking(FName("Enable_FootIK_L"), FName("Foot_Lock_L"), FName("ik_foot_l"), FootLock_L_Alpha, FootLock_L_Location, FootLock_L_Rotation);
	SetFootLocking(FName("Enable_FootIK_R"), FName("Foot_Lock_R"), FName("ik_foot_r"), FootLock_R_Alpha, FootLock_R_Location, FootLock_R_Rotation);

	/** Update Foot IK */
	FVector footOffset_L_Target;
	FVector footOffset_R_Target;

	SetFootOffset(FName("Enable_FootIK_L"), FName("ik_foot_l"), FName("Root"), footOffset_L_Target, FootOffset_L_Location, FoorOffset_L_Rotation);
	SetFootOffset(FName("Enable_FootIK_R"), FName("ik_foot_r"), FName("Root"), footOffset_R_Target, FootOffset_R_Location, FoorOffset_R_Rotation);

	SetPelvisIKOffset(footOffset_L_Target, footOffset_R_Target);
}					  
