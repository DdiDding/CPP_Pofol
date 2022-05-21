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
 * SubState��� ���õ� �������̽��Դϴ�.
 */
class CPP_POFOL_API ICAboutSubState
{
	GENERATED_BODY()

public:

	/**
	 * ACharacter�Ļ������� ��� SubState�� �����Ҷ� �� �Լ��� �Ἥ ����� �����ϴ�.
	 * �� SubState�ൿ�� �����ϸ� ���������� �˾Ƽ� �����̵ǰ� True�� ��ȯ�մϴ�.
	 * ���� �ൿ �Ұ����� ��Ȳ�̶�� False�� ��ȯ�մϴ�.
	 */
	virtual bool StartSubState() { return false; };
};
