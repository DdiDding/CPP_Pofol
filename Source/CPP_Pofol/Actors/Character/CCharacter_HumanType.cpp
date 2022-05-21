#include "CCharacter_HumanType.h"

ACCharacter_HumanType::ACCharacter_HumanType()
	:Super()
{

}

void ACCharacter_HumanType::BeginPlay()
{
	Super::BeginPlay();
}

void ACCharacter_HumanType::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


bool ACCharacter_HumanType::AddUpdateGroundedRotation()
{
	return Super::AddUpdateGroundedRotation();
}

UAnimMontage * ACCharacter_HumanType::GetGetUpAnimaMontage(bool isFaceUp)
{
	if (isFaceUp == true)
	{
		return GetUpAnim_FaceUp;
	}
	else
	{
		return GetUpAnim_FaceDown;
	}
}

void ACCharacter_HumanType::SetLocation()
{
	return;
}