#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CAboutSubState.generated.h"

UINTERFACE(MinimalAPI)
class UCAboutSubState : public UInterface
{
	GENERATED_BODY()
};

/**
 * SubState들과 관련된 인터페이스입니다.
 */
class CPP_POFOL_API ICAboutSubState
{
	GENERATED_BODY()

public:

	/**
	 * ACharacter파생형에서 어떠한 SubState를 실행할때 이 함수를 써서 명령을 내립니다.
	 * 그 SubState행동이 가능하면 내부적으로 알아서 실행이되고 True를 반환합니다.
	 * 만약 행동 불가능한 상황이라면 False를 반환합니다.
	 */
	virtual bool StartSubState() { return false; };
};
