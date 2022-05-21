#pragma once

#include "CoreMinimal.h"

/**
 * �ʿ��� ���� �Լ� ����
 */
class CPP_POFOL_API CMath
{
public:

	//���� Degree�� �����ش�.
	static float AngleBetweenVec(FVector vec1, FVector vec2);
	//��ȣ���ִ� Degree�� �����ش�.
	static float SignAngleBetweenVec(FVector vec1, FVector vec1upVec, FVector vec2);
	static bool LineToLineIntersection(const FVector& fromA, const FVector& fromB, const FVector& toA, const FVector& toB, FVector& _outIntersection);
};
