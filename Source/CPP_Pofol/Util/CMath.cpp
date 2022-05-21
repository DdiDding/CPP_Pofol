#include "Util/CMath.h"
#include "Kismet/KismetMathLibrary.h"

float CMath::AngleBetweenVec(FVector vec1, FVector vec2)
{
	vec1 = vec1.GetSafeNormal();
	vec2 = vec2.GetSafeNormal();

	return UKismetMathLibrary::DegAcos(FVector::DotProduct(vec1, vec2));
}


float CMath::SignAngleBetweenVec(FVector vec1, FVector vec1upVec, FVector vec2)
{
	vec1 = vec1.GetSafeNormal();
	vec2 = vec2.GetSafeNormal();

	float tempDegree = UKismetMathLibrary::DegAcos(FVector::DotProduct(vec1, vec2));

	FVector crossVec = FVector::CrossProduct(vec1, vec2);

	//내적값이 양수면 오른쪽
	if (0.0f <= FVector::DotProduct(crossVec, vec1upVec)) return tempDegree;
	else return tempDegree * -1.0f;
}

bool CMath::LineToLineIntersection(const FVector& fromA, const FVector& fromB, const FVector& toA, const FVector& toB, FVector& _outIntersection)
{
	FVector da = fromB - fromA;
	FVector db = toB - toA;
	FVector dc = toA - fromA;

	FVector crossDaDb = FVector::CrossProduct(da, db);
	float prod = crossDaDb.X * crossDaDb.X + crossDaDb.Y * crossDaDb.Y + crossDaDb.Z * crossDaDb.Z;
	if (prod == 0 || FVector::DotProduct(dc, crossDaDb) != 0)
	{
		return false;
	}
	float res = FVector::DotProduct(FVector::CrossProduct(dc, db), FVector::CrossProduct(da, db)) / prod;

	_outIntersection = fromA + da * FVector(res, res, res);

	FVector fromAToIntersectPoint = _outIntersection - fromA;
	FVector fromBToIntersectPoint = _outIntersection - fromB;
	FVector toAToIntersectPoint = _outIntersection - toA;
	FVector toBToIntersectPoint = _outIntersection - toB;
	if (FVector::DotProduct(fromAToIntersectPoint, fromBToIntersectPoint) <= 0 && FVector::DotProduct(toAToIntersectPoint, toBToIntersectPoint) <= 0)
	{
		return true;
	}
	return false;
}