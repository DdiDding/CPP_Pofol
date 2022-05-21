// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CNS_WallRunSetLocation.generated.h"


/**
 * WallRun의 애니메이션에서 벽을뚫지않게 캐릭터의 위치를 조정해주는 NotifyState
 */
UCLASS()
class CPP_POFOL_API UCNS_WallRunSetLocation : public UAnimNotifyState
{
	GENERATED_BODY()
public:

	UCNS_WallRunSetLocation();

	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;

protected:

	/**애니메이션이 벽에 뚫지않게 벽에서 띄어질 거리*/
	UPROPERTY(EditAnywhere)
	float wallOffSet;
};
