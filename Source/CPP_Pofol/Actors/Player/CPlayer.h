#pragma once
#include "CoreMinimal.h"
#include "Actors/Character/CCharacter_HumanType.h"
#include "Interfaces/CIInputFunc.h"
#include "CPlayer.generated.h"

class UCWallRunComponent;

UCLASS()
class CPP_POFOL_API ACPlayer : public ACCharacter_HumanType, public ICIInputFunc
{
	GENERATED_BODY()

public:
	ACPlayer();

////////////////////////////////////////////////////////////////////////////////
protected://Default Event
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) final;

////////////////////////////////////////////////////////////////////////////////
protected://Update
	
	/**** 간단한 변수의 업데이트*/
	void UpdateProperty();

////////////////////////////////////////////////////////////////////////////////
public://Default Property
	const FVector GetControllerForVec() { return this->controllerForVec; };

private:
	//컨트롤러의 전방 방향 벡터
	FVector controllerForVec;

////////////////////////////////////////////////////////////////////////////////
public://override Function

	/*UFUNCTION(BlueprintCallable)
	virtual UCDA_WeaponData* GetEquipWeaponData() override;*/

	virtual bool AddUpdateGroundedRotation() override;
	
	virtual void SetLocation() override;


////////////////////////////////////////////////////////////////////////////////
public://Input Key function

	//keyboard
	virtual void Move_FB(float axisValue) final;
	virtual void Move_RL(float axisValue) final;
	virtual void Press_Spacebar() final;
	virtual void Press_Shitf_L() final;
	virtual void Press_Ctrl() final;
	virtual void Press_Tab() final;

	//Bind skills
	virtual void Press_Q() final;
	virtual void Press_E() final;
	virtual void Press_R() final;
	virtual void Press_1() final;
	virtual void Press_2() final;
	virtual void Press_3() final;
	virtual void Press_4() final;

	//mouse
	virtual void Mouse_Vertical(float axisValue) final;
	virtual void Mouse_Horizontal(float axisValue) final;
	virtual void Press_Mouse_L() final;
	virtual void Press_Mouse_R() final;

	FVector2D MovementInput;
	FVector2D CameraInput;
	
////////////////////////////////////////////////////////////////////////////////
public://디버그

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
		bool bOnDebugMode = true;

	void Debug();


////////////////////////////////////////////////////////////////////////////////
public://SubMovementComponent
	UCWallRunComponent * GetWallRunComponent() { return wallRunComponent6; }

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	UCWallRunComponent * wallRunComponent6;

	float allTime{ 0.0f };

public:

	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestPublic();

	bool Test();

	void Test_True();
	void Test_False();
};





