#include "Structs/Character/CCharacterStruct.h"

UCCharacterStruct::UCCharacterStruct()
{
	//Setting eunmState
	{
		eMainState = EMainState::GROUND;
		eSubState = ESubState::NONE;
		eMoveMode = EMoveMode::RUNNING;
		eRotationMode = ERotationMode::NONE;
		eWeaponState = EWeaponState::NONE;
	}
}

////////////////////////////////////////////////////////////////////////////////
//EMainState
const EMainState UCCharacterStruct::GetMainState() {
	return eMainState;
}
void UCCharacterStruct::SetMainState_Ground()
{
	eMainState = EMainState::GROUND;
}
void UCCharacterStruct::SetMainState_Air()
{
	eMainState = EMainState::AIR;
}

////////////////////////////////////////////////////////////////////////////////
//ESubState
const ESubState UCCharacterStruct::GetSubState()
{
	return eSubState;
}

void UCCharacterStruct::SetSubState_None()
{
	eSubState = ESubState::NONE;
}

void UCCharacterStruct::SetSubState_Wallrun()
{
	eSubState = ESubState::WALLRUN;
}

void UCCharacterStruct::SetSubState_Attack()
{
	eSubState = ESubState::ATTACK;
}

void UCCharacterStruct::SetSubState_Hitted()
{
	eSubState = ESubState::HITTED;
}

void UCCharacterStruct::SetSubState_LayDown()
{
	eSubState = ESubState::LAY_DOWN;
}

void UCCharacterStruct::SetSubState_Grappling()
{
	eSubState = ESubState::GRAPPLING;
}

void UCCharacterStruct::SetSubState_GetUp()
{
	eSubState = ESubState::GET_UP;
}


////////////////////////////////////////////////////////////////////////////////
//EMoveMode
const EMoveMode UCCharacterStruct::GetMoveMode()
{
	return eMoveMode;
}
void UCCharacterStruct::SetMoveMode_Walking()
{
	eMoveMode = EMoveMode::WALKING;
}
void UCCharacterStruct::SetMoveMode_Running()
{
	eMoveMode = EMoveMode::RUNNING;
}


////////////////////////////////////////////////////////////////////////////////
//Rotation Mode
void UCCharacterStruct::SetRotationMode_None() {
	eRotationMode = ERotationMode::NONE;
}
void UCCharacterStruct::SetRotationMode_Velocity() {

	/*GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;*/
	eRotationMode = ERotationMode::VELOCITY;
}
void UCCharacterStruct::SetRotationMode_Controller() {

	/*GetCharacterMovement()->bOrientRotationToMovement = false;
	this->bUseControllerRotationYaw = true;*/
	eRotationMode = ERotationMode::CONTROLLER;
}
const ERotationMode UCCharacterStruct::GetRotationMode() {
	return eRotationMode;
}


////////////////////////////////////////////////////////////////////////////////
//EWeaponState
const EWeaponState UCCharacterStruct::GetWeaponState() {
	return eWeaponState;
}
void UCCharacterStruct::SetDefault() {
	eWeaponState = EWeaponState::NONE;
}
void UCCharacterStruct::SetGreatSword() {
	eWeaponState = EWeaponState::GREATSWORD;
}
void UCCharacterStruct::SetTwinDagger() {
	eWeaponState = EWeaponState::TWINDAGGER;
}
