#include "Components/CBehaviorTraceManagerComponent.h"

// Sets default values for this component's properties
UCBehaviorTraceManagerComponent::UCBehaviorTraceManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//���� ������ ���۵� ����
	//Ʈ���̽� �߰�?
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
//	//Main state�� ����
//	//if (���̶��)
//	//	GetWorld()->LineTraceSingleByChannel(
//	//		HitResult,
//	//		startTraceLocation,
//	//		endTraceLocaiton,
//	//		ECollisionChannel::ECC_Visibility,
//	//		TraceParams);
//
//	//	Trace2���� -> 
//	//	Trace3���� -> 
//	//	
//
//
//	//	//�̰� �Լ�ȭ
//	//	if (1�� 3�� true ? )
//	//	{
//	//		�ൿ A = true
//	//	}
//
//	//if (2�� 3�� true ? )
//	//{
//	//	�ൿ B = true
//	//}
//
//}
//
