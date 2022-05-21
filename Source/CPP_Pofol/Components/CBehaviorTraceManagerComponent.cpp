#include "Components/CBehaviorTraceManagerComponent.h"

// Sets default values for this component's properties
UCBehaviorTraceManagerComponent::UCBehaviorTraceManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//실행 가능한 동작들 세팅
	//트레이스 추가?
}


// Called when the game starts
void UCBehaviorTraceManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCBehaviorTraceManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


//void UCBehaviorTraceManagerComponent::UpdateTrace()
//{
//	//Main state에 따른
//	//if (땅이라면)
//	//	GetWorld()->LineTraceSingleByChannel(
//	//		HitResult,
//	//		startTraceLocation,
//	//		endTraceLocaiton,
//	//		ECollisionChannel::ECC_Visibility,
//	//		TraceParams);
//
//	//	Trace2실행 -> 
//	//	Trace3실행 -> 
//	//	
//
//
//	//	//이건 함수화
//	//	if (1번 3번 true ? )
//	//	{
//	//		행동 A = true
//	//	}
//
//	//if (2번 3번 true ? )
//	//{
//	//	행동 B = true
//	//}
//
//}
//
