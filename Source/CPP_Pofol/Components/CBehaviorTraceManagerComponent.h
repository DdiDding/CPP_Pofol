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
////����� ���۷����ι���!!����!!!
////�ڼ��Ѱ� ���� �Ұ��̰�
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

	//�Լ��� �����ڿ� �Ἥ ���� �߰�

	//�̸��� bool �� ������ ?
	//void AddBehavior(FString behaviorName, �̷��� Ʈ���̽� �䱸1, �̷��� Ʈ���̽� �䱸2);

	/*void DoTrace();

	void UpdateTrace();

	bool ReturnBoolOfBehavior(FString behaviorName);*/

private:

	/**
	*��� Ʈ���̽� ���ǵ��� ��� �迭
	*/
	//Tarray<FCTraceParameter> traceParameters;
};