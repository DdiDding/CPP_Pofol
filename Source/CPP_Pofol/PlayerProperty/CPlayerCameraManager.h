#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CPlayerCameraManager.generated.h"

class USkeletalMeshComponent;
class UPostProcessComponent;
class UMaterialInstanceDynamic;
/**
 * 
 */
UCLASS()
class CPP_POFOL_API ACPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ACPlayerCameraManager();
	//PlayerController���� ȣ�� ��
	//void BeginPlay();
	void OnPosess();


////////////////////////////////////////////////////////////////////////////////
private://components

	//���¿����� ���� �����ϱ����� �ִ� �ν��Ͻ����� Ŀ�갪 �̿��ϱ� ���� ���̷�Ż �޽�
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Component", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent * customCamera2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Component", meta = (AllowPrivateAccess = true))
	UPostProcessComponent * postprocessComponent;

////////////////////////////////////////////////////////////////////////////////
public://Public

	/**
	 *	�� ��û�� ��� �Դٸ� ���� Ŀ�갪�� ���� �����մϴ�.
	 *	NotifyState�� ����˴ϴ�.
	 */
	UFUNCTION(BlueprintCallable)
	void SetBlurFromCurve(float curveValue);

	UFUNCTION(BlueprintCallable)
	void SetExposureCompensation(float value);

	UFUNCTION(BlueprintCallable)
	void SetExposureCompensation_Zero();

////////////////////////////////////////////////////////////////////////////////
private://Updates

	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) final;

		/*** ����ī�޶��� Transform�� ���մϴ�. */
		void CalCustomCameraTrans(FVector& targetLocation, FRotator& targetRotation, float& targetFOV);

			/****
			 * Lag�� ���� �� ī�޶��� ��ġ�� ��ȯ�մϴ�.
			 * @param cameraTransform - ���� ī�޶� ��ġ
			 * @param targetTransform - ���� ��Ÿ�پ����� ��ġ
			 * @param targetRoation - ���������� ������ Lag�� ����ϱ� ���� ����ϴ� ȸ����
			 */
			FVector CalculateLag(FTransform cameraTransform, FTransform targetTransform, FRotator finalRoation);

			float GetCurveValue(FName curveName);

	


////////////////////////////////////////////////////////////////////////////////
private://member variable

	UPROPERTY()
	AActor * targetActor;

	FTransform cameraTransform;

	FVector CameraPivotLocation;
	FVector finalCameraLocation;
	FRotator finalCameraRotation;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FPostProcessSettings postprocessSetting;

	//0���� ��, 1���� ��Ÿ���
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FWeightedBlendable> postprocessMaterialArray;

	UMaterialInterface * blurMat;
	UMaterialInstanceDynamic * blurMatInst;
	float maxBlurValue;

};
