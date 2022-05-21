#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class ICIInputFunc;
/**
 * 
 */
UCLASS()
class CPP_POFOL_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACPlayerController();

protected:
	virtual void BeginPlay() final;
	virtual void OnPossess(APawn* aPawn) final;
	//virtual void BeginPlayingState() final;
	//virtual void OnUnPossess() final;

private:
	virtual void SetupInputComponent() final;
	virtual void PlayerTick(float DeltaTime) final;

////////////////////////////////////////////////////////////////////////////////
private://각 입력 함수 (함수 내부에서 myPawn의 인터페이스 호출)
	//keyboard
	void Move_FB(float axisValue);
	void Move_RL(float axisValue);
	void Press_Spacebar();
	void Press_Shitf_L();
	void Press_Ctrl();
	void Press_Tab();

	//bind skill
	void Press_Q();
	void Press_E();
	void Press_R();
	void Press_1();
	void Press_2();
	void Press_3();
	void Press_4();

	//mouse
	void Mouse_Vertical(float axisValue);
	void Mouse_Horizontal(float axisValue);
	void Press_Mouse_L();
	void Press_Mouse_R();

////////////////////////////////////////////////////////////////////////////////
private://member variable
	ICIInputFunc * myPawn;

};
