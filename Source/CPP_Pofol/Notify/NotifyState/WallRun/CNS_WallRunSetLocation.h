// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CNS_WallRunSetLocation.generated.h"


/**
 * WallRun�� �ִϸ��̼ǿ��� ���������ʰ� ĳ������ ��ġ�� �������ִ� NotifyState
 */
UCLASS()
class CPP_POFOL_API UCNS_WallRunSetLocation : public UAnimNotifyState
{
	GENERATED_BODY()
public:

	UCNS_WallRunSetLocation();

	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;

protected:

	/**�ִϸ��̼��� ���� �����ʰ� ������ ����� �Ÿ�*/
	UPROPERTY(EditAnywhere)
	float wallOffSet;
};
