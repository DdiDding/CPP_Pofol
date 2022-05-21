#pragma once

#include "CoreMinimal.h"

/**
 * 필요한 수학 함수 모음
 */
class CPP_POFOL_API CMath
{
public:

	//절대 Degree를 구해준다.
	static float AngleBetweenVec(FVector vec1, FVector vec2);
	//부호가있는 Degree를 구해준다.
	static float SignAngleBetweenVec(FVector vec1, FVector vec1upVec, FVector vec2);
	static bool LineToLineIntersection(const FVector& fromA, const FVector& fromB, const FVector& toA, const FVector& toB, FVector& _outIntersection);
};
