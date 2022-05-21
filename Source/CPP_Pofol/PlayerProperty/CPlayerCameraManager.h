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
	//PlayerController에서 호출 됨
	//void BeginPlay();
	void OnPosess();


////////////////////////////////////////////////////////////////////////////////
private://components

	//상태에따른 값을 블랜딩하기위해 애님 인스턴스안의 커브값 이용하기 위한 스켈레탈 메시
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Component", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent * customCamera2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Component", meta = (AllowPrivateAccess = true))
	UPostProcessComponent * postprocessComponent;

////////////////////////////////////////////////////////////////////////////////
public://Public

	/**
	 *	블러 요청이 들어 왔다면 블러를 커브값에 따라 조절합니다.
	 *	NotifyState로 실행됩니다.
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

		/*** 최종카메라의 Transform을 구합니다. */
		void CalCustomCameraTrans(FVector& targetLocation, FRotator& targetRotation, float& targetFOV);

			/****
			 * Lag가 적용 된 카메라의 위치를 반환합니다.
			 * @param cameraTransform - 현재 카메라 위치
			 * @param targetTransform - 현재 뷰타겟액터의 위치
			 * @param targetRoation - 절대축으로 돌려서 Lag를 계산하기 위해 사용하는 회전값
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

	//0번은 블러, 1번은 기타등등
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FWeightedBlendable> postprocessMaterialArray;

	UMaterialInterface * blurMat;
	UMaterialInstanceDynamic * blurMatInst;
	float maxBlurValue;

};
