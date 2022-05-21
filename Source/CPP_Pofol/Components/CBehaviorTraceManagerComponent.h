// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CBehaviorTraceManagerComponent.generated.h"


//
//enum class ETraceType :uint8
//{
//	Line,
//	Sphere,
//	Capsule,
//	Max
//};
//
////결과를 레퍼런스로받자!!ㅆㅃ!!!
////자세한건 따로 할것이고
//USTRUCT()
//struct CPP_POFOL_API FCTraceParameter
//{
//	GENERATED_BODY()
//public:
//	FCTraceParameter()
//private:
//	FName name;
//	FVector startLocation;
//	FVector endLocation;
//
//
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_POFOL_API UCBehaviorTraceManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCBehaviorTraceManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//함수를 생성자에 써서 쉽게 추가

	//이름과 bool 을 맵으로 ?
	//void AddBehavior(FString behaviorName, 이러한 트레이스 요구1, 이러한 트레이스 요구2);

	/*void DoTrace();

	void UpdateTrace();

	bool ReturnBoolOfBehavior(FString behaviorName);*/

private:

	/**
	*모든 트레이스 조건들이 담길 배열
	*/
	//Tarray<FCTraceParameter> traceParameters;
};