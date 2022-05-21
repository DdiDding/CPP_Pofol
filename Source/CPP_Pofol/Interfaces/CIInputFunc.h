#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CIInputFunc.generated.h"

UINTERFACE(MinimalAPI)
class UCIInputFunc : public UInterface
{
	GENERATED_BODY()
};

/**
 * Player controller에서 누르면 실행될 함수들 입니다.
 * 여러 캐릭터에 빙의할 경우를 생각하여 인터페이스로 분리합니다.
 */
class CPP_POFOL_API ICIInputFunc
{
	GENERATED_BODY()

public:

	//keyboard
	virtual void Move_FB(float axisValue) {};
	virtual void Move_RL(float axisValue) {};
	virtual void Press_Spacebar() {};
	virtual void Press_Shitf_L() {};
	virtual void Press_Ctrl() {};
	virtual void Press_Tab() {};

	virtual void Press_Q(){};
	virtual void Press_E(){};
	virtual void Press_R(){};
	virtual void Press_1(){};
	virtual void Press_2(){};
	virtual void Press_3(){};
	virtual void Press_4(){};

	//mouse
	virtual void Mouse_Vertical(float axisValue) {};
	virtual void Mouse_Horizontal(float axisValue) {};
	virtual void Press_Mouse_L() {};
	virtual void Press_Mouse_R() {};
};
