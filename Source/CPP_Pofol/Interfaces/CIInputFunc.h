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
 * Player controller���� ������ ����� �Լ��� �Դϴ�.
 * ���� ĳ���Ϳ� ������ ��츦 �����Ͽ� �������̽��� �и��մϴ�.
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
