// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Enemy/TestEnemy/CTestEnemy.h"

ACTestEnemy::ACTestEnemy()
{
	//SkeletalMesh Init
	{
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}
}

void ACTestEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ACTestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}