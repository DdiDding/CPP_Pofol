#include "PlayerProperty/CPlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Util/CLog.h"

ACPlayerCameraManager::ACPlayerCameraManager()
{
	//Setting Components
	{
		postprocessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostPocess Component"));
		check(postprocessComponent);

		customCamera2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Custom Camera"));
		check(customCamera2);
	}

	//Setting AnimInstance
	{
		customCamera2->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance>CameraAnim(TEXT("AnimBlueprint'/Game/Actors/Camera/BPA_Camera.BPA_Camera_C'"));
		customCamera2->SetAnimInstanceClass(CameraAnim.Class);
	}

	//SkeletalMesh Init
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>CameraSkel(TEXT("SkeletalMesh'/Game/Resources/Skeletals/CameraSkele/Camera.Camera'"));
		customCamera2->SetSkeletalMesh(CameraSkel.Object);
	}

	//Blur material 가져오기
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>Blur(TEXT("MaterialInstanceConstant'/Game/Resources/PostProcess/Blur/MI_Blur.MI_Blur'"));
	blurMat = Blur.Object;

	//MaterialParameterCollection 가져오기
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection>MPC_Blur(TEXT("MaterialParameterCollection'/Game/Resources/PostProcess/Blur/MPC_Blur.MPC_Blur'"));
	
	maxBlurValue = MPC_Blur.Object->GetScalarParameterByName(FName("MaxBlur"))->DefaultValue;
}

//void ACPlayerCameraManager::BeginPlay()
//{
//}

void ACPlayerCameraManager::OnPosess()
{
	targetActor = GetViewTarget();

	//Postprocess Init
	{
		blurMatInst = UMaterialInstanceDynamic::Create(blurMat, this);
		blurMatInst->SetScalarParameterValue(FName("Blur"), 0.0f);

		postprocessMaterialArray.Add(FWeightedBlendable(1.0f, blurMatInst));
		postprocessMaterialArray.Add(FWeightedBlendable());

		postprocessComponent->Settings.WeightedBlendables = FWeightedBlendables(postprocessMaterialArray);
	
		postprocessComponent->Settings.AutoExposureBias = 0.0f;
	}
}

void ACPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	if (OutVT.Target)
	{
		FVector OutLocation;
		FRotator OutRotation;
		float OutFOV;
		CalCustomCameraTrans(OutLocation, OutRotation, OutFOV);

		OutVT.POV.Location = OutLocation;
		OutVT.POV.Rotation = OutRotation;
		OutVT.POV.FOV = OutFOV;
	}
	else 
	{
		Super::UpdateViewTargetInternal(OutVT, DeltaTime);
	}
}

void ACPlayerCameraManager::CalCustomCameraTrans(FVector& finalLocation, FRotator& finalRotation, float& targetFOV)
{
	
	/** ||최종 회전 값을 구합니다.||
	 *	targetTransform	: 컨트롤중인 액터의 트랜스폼
	 */
	FTransform targetTransform = targetActor->GetActorTransform();


	/** ||최종 회전 값을 구합니다.||*/
	finalCameraRotation = FMath::RInterpTo(
		GetCameraRotation(), GetOwningPlayerController()->GetControlRotation(),
		GetWorld()->DeltaTimeSeconds, GetCurveValue("RotationLagSpeed")
	);


	/** ||보간해가는(Lag) 카메라의 Transform을 구합니다.||*/
	cameraTransform = FTransform(
		targetTransform.GetRotation(),
		CalculateLag(cameraTransform, targetTransform, finalCameraRotation),
		FVector(1.0f, 1.0f, 1.0f)
	);
	
	/** ||PivotOffset을 계산하여 더하면 카메라의 중심 위치입니다.||*/
	FVector tempX = cameraTransform.GetRotation().GetForwardVector() * GetCurveValue("PivotOffset_X");
	FVector tempY = cameraTransform.GetRotation().GetRightVector() * GetCurveValue("PivotOffset_Y");
	FVector tempZ = cameraTransform.GetRotation().GetUpVector() * GetCurveValue("PivotOffset_Z");

	CameraPivotLocation = cameraTransform.GetLocation() + tempX + tempY + tempZ;

	/** ||카메라의 중심 위치에서 캐릭터를 중심으로 얼마나 떨어져있을건지 offset을 계산합니다.||*/
	tempX = FQuat(finalCameraRotation).GetForwardVector() * GetCurveValue("CameraOffset_X");
	tempY = FQuat(finalCameraRotation).GetRightVector() * GetCurveValue("CameraOffset_Y");
	tempZ = FQuat(finalCameraRotation).GetUpVector() * GetCurveValue("CameraOffset_Z");

	finalCameraLocation = CameraPivotLocation + tempX + tempY + tempZ;

	/** ||.||*/
	finalLocation = finalCameraLocation;
	finalRotation = finalCameraRotation;
	targetFOV = 90.0f;
}

FVector ACPlayerCameraManager::CalculateLag(FTransform cameraTrans, FTransform targetTrans, FRotator finalRoation)
{
	FVector resultVec;
	FVector LagSpeed = FVector(
		GetCurveValue("PivotLagSpeed_X"),
		GetCurveValue("PivotLagSpeed_Y"),
		GetCurveValue("PivotLagSpeed_Z")
	);
	
	//절대축으로 돌리기위해 Yaw만 가져온다.
	FRotator rotateValue = FRotator(0.0f, finalRoation.Yaw, 0.0f);

	FVector currentVec = rotateValue.UnrotateVector(cameraTrans.GetLocation());
	FVector targetVec = rotateValue.UnrotateVector(targetTrans.GetLocation());

	//VInterpTo를 안쓰는 이유는 LagSpeed가 각 축마다 모두 달라서
	resultVec.X = FMath::FInterpTo(currentVec.X, targetVec.X, GetWorld()->DeltaTimeSeconds, LagSpeed.X);
	resultVec.Y = FMath::FInterpTo(currentVec.Y, targetVec.Y, GetWorld()->DeltaTimeSeconds, LagSpeed.Y);
	resultVec.Z = FMath::FInterpTo(currentVec.Z, targetVec.Z, GetWorld()->DeltaTimeSeconds, LagSpeed.Z);

	return rotateValue.RotateVector(resultVec);
}

float ACPlayerCameraManager::GetCurveValue(FName curveName)
{
	if (customCamera2->GetAnimInstance() == nullptr) return 0.0f;
	return customCamera2->GetAnimInstance()->GetCurveValue(curveName);
}

void ACPlayerCameraManager::SetBlurFromCurve(float curveValue)
{
	blurMatInst->SetScalarParameterValue(FName("Blur"), curveValue);
}

void ACPlayerCameraManager::SetExposureCompensation(float value)
{
	postprocessComponent->Settings.AutoExposureBias = value;
	postprocessComponent->Settings.AutoExposureSpeedUp = 10.0f;
	postprocessComponent->Settings.AutoExposureSpeedDown = 13.0f;
}

void ACPlayerCameraManager::SetExposureCompensation_Zero()
{
	postprocessComponent->Settings.AutoExposureBias = 0.0f;
	postprocessComponent->Settings.AutoExposureSpeedUp = 3.0f;
	postprocessComponent->Settings.AutoExposureSpeedDown = 1.0f;
}
