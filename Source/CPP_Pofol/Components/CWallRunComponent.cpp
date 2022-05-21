#include "CWallRunComponent.h"
#include "Actors/Player/CPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Util/CLog.h"
#include "Util/CMath.h"
#include <Kismet/KismetSystemLibrary.h>


UCWallRunComponent::UCWallRunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	owner = Cast<ACPlayer>(GetOwner());

	finalWall.Init();
	finalWall.Distance = radius;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage>LEFT_WALLJUMP(TEXT("AnimMontage'/Game/Actors/Player/Animation/WallRun/WallJump/M_wallJumpLeft.M_wallJumpLeft'"));
	leftWallJump = LEFT_WALLJUMP.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>RIGHT_WALLJUMP(TEXT("AnimMontage'/Game/Actors/Player/Animation/WallRun/WallJump/M_wallJumpRight.M_wallJumpRight'"));
	rightWallJump = RIGHT_WALLJUMP.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>VERTICAL_WALLJUMP(TEXT("AnimMontage'/Game/Actors/Player/Animation/WallRun/WallJump/M_wallJumpVertical.M_wallJumpVertical'"));
	verticalWallJump = VERTICAL_WALLJUMP.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>WALL_EXIT(TEXT("AnimMontage'/Game/Actors/Player/Animation/WallRun/M_Wall_Exit.M_Wall_Exit'"));
	wallExit = WALL_EXIT.Object;
}

void UCWallRunComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	deltaTime = DeltaTime;

	DEBUGWALLRUN{ Debug(); 	}

	if (eWallRunState != EWallRunState::DOING) return;
	
	//EWallRunState::DOING�϶��� ������Ʈ
	UpdateDoingWallRun();
}

void UCWallRunComponent::UpdateDoingWallRun()
{
	if (UpdateIsPossibleWallRunState() == false) { return; }

	UpdateProperty();
	UpdateProjectWallAxes();
	UpdateMovingDegree();
}

bool UCWallRunComponent::UpdateIsOnGround() 
{
	//TODO : ���߿� �̰� Character�� ���� ���⼭ MainSTate�� Ground���� Ȯ���ؾ��Ҽ�������
	FFindFloorResult result;
	owner->GetCharacterMovement()->FindFloor(owner->GetActorLocation(), result, false);

	if (FMath::IsNearlyZero(result.FloorDist, 0.05f)) { return true; }
	else return false;
}

bool UCWallRunComponent::UpdateFinalWall()
{
	/**	||���� �����ϱ� ���� Ʈ���̽��� ����մϴ�.|| 
	 *	ownerLoc	: ĳ������ ���� ��ġ
	 *	tempHits		: WallRunä���� ��� 1�������� ��� ��Ÿ�Ⱑ ������ ���� �����մϴ�.
	 *	arrayWallInRange: 2�������� tempHits���� ĳ���� ���� 180�� �ȿ� �ִ� ���� ������ �����մϴ�.
	 */	
	FVector ownerLoc = owner->GetActorLocation();
	TArray<FHitResult> tempHits;
	TArray<FHitResult> arrayWallInRange;

	if (GetWorld()->SweepMultiByChannel(tempHits, ownerLoc,
		ownerLoc + FVector(0, 0, 1), FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeCapsule(radius, halfHeight)) == false)
	{
		finalWall.bBlockingHit = false;
		return false;
	}

	
	for (int i = 0; i < tempHits.Num(); i++)
	{
		if(0.9 <= FVector::DotProduct(tempHits[i].ImpactNormal, FVector(0.0f,0.0f,1.0f)))
		{///���������ʿ� �ִ� ���� �������� �ʽ��ϴ�.
			continue;
		}
		if (FVector::DotProduct(owner->GetActorForwardVector().GetSafeNormal(), tempHits[i].ImpactNormal) <= 0.2f)
		{///���� 180�� �ȿ� ������ �� ������ �����մϴ�.
			arrayWallInRange.Emplace(tempHits[i]);
		}
	}


	if (arrayWallInRange.Num() == 0)
	{
		finalWall.bBlockingHit = false;
		return false;
	}
	
	
	/**	||���������� ���� ����� ���� ã���ϴ�.||
	 *	minLength	: ���� ����� �Ÿ��� ������Ʈ �ϱ� ���� ��, 1�� ���� �ݰ��� radius�̹Ƿ� �⺻������ ����.
	 */
	
	float minLength = radius;

	for (FHitResult & tempResult : arrayWallInRange)
	{
		float tempLength = (tempResult.ImpactPoint - ownerLoc).Size2D();

		if (tempLength < minLength)
		{
			minLength = tempLength;
			finalWall = tempResult;
		}
	}


	/**	||�÷��̾��� �ٸ��ʿ��� ��� ������ �ȵǸ� ��ݽŸ� �پ� �ִ� �� �̹Ƿ� ������ ������ ġ�� �ʽ��ϴ�.||
	 *	
	 */
	FHitResult tempHit;
	FVector startLoc = FVector(ownerLoc.X, ownerLoc.Y, ownerLoc.Z - 44.0f);

	if (GetWorld()->LineTraceSingleByChannel(tempHit,
		startLoc, startLoc + (finalWall.ImpactNormal * -1.0f) * radius,
		ECollisionChannel::ECC_GameTraceChannel1) == false)
	{///Ʈ���̽��� �ƹ��͵� ���� ������ �� ���� ����
		return false;
	}


	/**	||�÷��̾��� ����������� �����ִ� ���� �ִ��� Ʈ���̽��� ����� Ȯ���մϴ�.|| 
	 *	Ʈ���̽����� ������ �Ǵ� ������ ����Ǵ� �κа� �� ���� ƽ���Ϳ��� �� ����Ǵ� �κ����� ���������ֽ��ϴ�.
	 *	ó�� ���� ������ ����Ǵ� �κп����� ���� �ִ���, �� ���� Ʈ���̽��� ����ɶ� �ʿ��� �����͸� ���� �մϴ�.
	 *	
	 *	lineHit				: �𼭸��� ��ġ ����
	 *	lineTraceDir		: Ʈ���̽��� ����� ����
	 *	oppositeWallBoostDir: �𼭸��� ���� ������ ������ �̵��ϱ����� �ν�Ʈ ����
	 */
	FHitResult lineHit;
	FVector lineTraceDir = FVector::ZeroVector;
	FVector oppositeWallBoostDir = FVector::ZeroVector;

	if (bLastHitTrace == false)
	{
		
		lineTraceDir = (FVector::VectorPlaneProject(owner->GetVelocity(), finalWall.ImpactNormal)).GetSafeNormal2D();
		
		if (GetWorld()->LineTraceSingleByChannel(lineHit,
			ownerLoc, ownerLoc + lineTraceDir * radius,
			ECollisionChannel::ECC_GameTraceChannel1) == false)
		{///Ʈ���̽��� �ƹ��͵� ���� ������ �� ���� ����
			return true;
		}
		
		cornerLoc = lineHit.ImpactPoint;
		oppositeWallBoostDir = FVector::VectorPlaneProject(finalWall.ImpactNormal, lineHit.ImpactNormal).GetSafeNormal2D();

		//���� ���� ���� ����
		calBeforeStartDir = (ownerLoc - cornerLoc).GetSafeNormal2D();
		calBeforeDegree = CMath::AngleBetweenVec(calBeforeStartDir, oppositeWallBoostDir);

		//���� �� �� Ʈ���̽������ϰ� ������ �׳� �ϱ�
		if (maxTraceDegree < calBeforeDegree)
		{
			return true;
		}

		calAfterStartDir = finalWall.ImpactNormal * -1.0f;
		calAfterEndDir = lineHit.ImpactNormal * -1.0f;
		

		/**	||���� ������ �ε巴�� ���� ���� �ν�Ʈ�� �ݴϴ�.||
		 *	boostDir	: �ν�Ʈ ���⺤�͸� ���Ҷ� �� ���� ����
		 *	boostSpeed	: �ν�Ʈ ���ǵ�
		 */
		FVector boostDir = (cornerLoc + (oppositeWallBoostDir * radius * 3.0f) - ownerLoc).GetSafeNormal2D();
		//TODO : ������ ���� ����, Velocity�� ���� ����
		const float boostSpeed =
			50000.0f///�⺻ ��
			+ (2000.0f / calBeforeDegree)///������ ���� ����
			+ (owner->GetVelocity().Size() * 40.0f);///ĳ���� ���ν�Ƽ�� ���� ����
		owner->GetCharacterMovement()->AddImpulse(boostDir * boostSpeed);

		bLastHitTrace = true;
	}
	if (bLastHitTrace == true)
	{
		/**	||�� ������ ����, ���� ��ֳ����� ������ ���� ���� �̿��Ͽ� Impact Normal�� �����մϴ�.||
		*	1.���� ĳ������ġ�� �� ������ �������� �󸶸�ŭ�� ������ ������ ���ϰ� ����ȭ�� ���� ���մϴ�.
		*	2.�� ���� ���� ���� ��ֳ����� �������� ���� ���� ���մϴ�. �� ���� Impact Normal���� �˴ϴ�.
		*	
		*	tempCorner : ���� ������ Corner�� Z���� ������Ʈ�� �ȵǼ� ���� ������Ʈ ���� Corner��
		*/
		FVector tempCorner = FVector(cornerLoc.X, cornerLoc.Y, ownerLoc.Z);
		lineTraceDir = (tempCorner - ownerLoc).GetSafeNormal2D();

		if (GetWorld()->LineTraceSingleByChannel(lineHit,
			ownerLoc, ownerLoc + lineTraceDir * radius,
			ECollisionChannel::ECC_GameTraceChannel1) == false)
		{///Ʈ���̽��� �ƹ��͵� ���� ������ �� ���� ����
			bLastHitTrace = false;
			return true;
		}
		
		float angleAlhpa = CMath::AngleBetweenVec(calBeforeStartDir, (ownerLoc - tempCorner).GetSafeNormal2D());
		angleAlhpa = UKismetMathLibrary::NormalizeToRange(angleAlhpa, 0.0f, calBeforeDegree);
		angleAlhpa = FMath::Clamp(angleAlhpa, 0.0f, 1.0f);

		FVector tempNorm = FMath::Lerp(calAfterStartDir, calAfterEndDir, angleAlhpa);
		finalWall.ImpactNormal = (tempNorm * -1.0f).GetSafeNormal2D();
	}

	return true;
}

bool UCWallRunComponent::UpdateIsPossibleWallRunState()
{
	if (finalWall.bBlockingHit == false)
	{
		return false;
	}

	/** ||WallRun�߿� ���� �پ����� Ȯ���ϰ� ĳ������ �ӵ��� ���� �ӵ� ���ϸ� WallRun���� ����˴ϴ�.|| */
	if (UpdateIsOnGround() == true || owner->GetVelocity().Size() < minimumOwnerSpeed)
	{
		owner->GetCharacterStruct()->SetMainState_Ground();
		EndWallRun();
		return false;
	}
	

	/** ||��Ÿ���߿� ���� �ٴٶ��� �� �˸��� ó���� �ϰ� WallRUn�� �����ϴ�.||
	 *	1.�������� ���� upVec�� ������ 0.5�̻��̸� �������� �ö󰣴ٷ� �����Ͽ� ó���մϴ�.
	 *	2.������ �Ѿ� ���� �������� ĳ���͸� �о���ϴ�. ĳ������ Velocity�� ������ �ݴϴ�.
	 *	3.�Ѿ�� ��Ÿ�� ����մϴ�.
	 */
	if (0.65f <= FVector::DotProduct(finalWall.ImpactNormal, FVector(0.0f, 0.0f, 1.0f)))
	{
		FVector tempDir = FVector::VectorPlaneProject(finalWall.ImpactNormal * -1.0f, FVector(0.0f, 0.0f, 1.0f)).GetSafeNormal();
		FVector boostDir = owner->GetVelocity().GetSafeNormal2D();

		boostDir += tempDir * 0.7f; ///�ö󰡴� ���� �������� 0.7��ŭ ���͸� �����ش�.
		boostDir += FVector(0.0f, 0.0f, 0.6f); ///���� 0.8��ŭ ���͸� �����ش�
		owner->GetCharacterMovement()->AddImpulse(boostDir*130000.0f);
		
		owner->PlayAnimMontage(wallExit);
		EndWallRun();
		return false;
	}


	//�������� ���˻�� ���ÿ� �� ������ ����
	if (UpdateFinalWall() == false)
	{
		EndWallRun();
		return false;
	}

	return true;
}

bool UCWallRunComponent::StartSubState()
{

	if(eWallRunState == EWallRunState::DOING)
	{
		WallJump();
		EndWallRun();
		//Ű������ ������
		//Ű�ȴ����� �����̽��ٸ� ���ڸ� ��������
		return true;
	}

	//������ ���� �ִ��� Ȯ���Ѵ�.
	if (UpdateFinalWall() == false)
	{
		eWallRunState = EWallRunState::IMPOSSIBLE;
		return false;
	}
	else
	{
		StartWallRun();
		return true;
	}
}

void UCWallRunComponent::UpdateProperty()
{
	upVecProjectWall = FVector::VectorPlaneProject(FVector(0.0f, 0.0f, 1.0f), finalWall.ImpactNormal);
}

void UCWallRunComponent::UpdateProjectWallAxes()
{
	/**	||�ι� �̻� ���� �Ǵ� ����||
	 *	wallNorProjectGround		: ��Normal�� ���ٴڿ� ������Ų ����
	 *	revWallNormProjectGround	: ��(wallNorProjectGround)�� ������
	 *	xOneVec						: x�࿡ ������ ����
	 *	tempVec						: ��� ��Ƶδ� ����
	 *	forwardVec					: W�� ������ �� �̵��Ϸ��� ������ ���� ����
	 *	upVec						: ���� Normal, Ȥ�� �� ������
	 *	rightVec					: D�� ������ �� �̵��Ϸ��� ������ ���� ����
	 *	backBoundaryValue			: ��Ʈ�ѷ��� �� �������ΰ��� ���� ���������µ� �̰� �������� �Ǻ��ϴµ� 0�� �������� �ϸ�
	 *								  ���ڿ��������� �ڿ������� �ϱ� ���� ���� ���� �����̴� �ϴ� ���� ��� ��
	 */
	FVector wallNorProjectGround = FVector::VectorPlaneProject(finalWall.ImpactNormal, FVector(0.0f, 0.0f, 1.0f));
	FVector revWallNormProjectGround = wallNorProjectGround * -1;
	FVector xOneVec = FVector(1.0f, 0.0f, 0.0f);
	FVector tempVec = FVector::ZeroVector;
	FVector forwardVec = FVector::ZeroVector;
	FVector upVec = FVector::ZeroVector;
	FVector rightVec = FVector::ZeroVector;
	float backBoundaryValue = -0.8f;


	/** ||���� Normal���� ��Ʈ�ѷ��� �����Ͱ� ��ŭ�� ������ �������ִ���, �� ������ X�����鿡�� ȸ����Ų ���͸� ���մϴ�.||
	 *	1. ��Ʈ�ѷ��� ������ ���͸� ���Ѵ�. = ���� hitLocation���� ��Ʈ�ѷ������� ���ϴ� ����
	 *	2. ���� Normal���� 1���� ���͸� �̿��� �� ���̰��� ��ȣ�� ���� ���� ������ ���Ѵ�.
	 *	3. ������ �̿��ؼ� ��ȣ�� ���Ͽ� X���� ���� ���͸� �����ش�. = �Է¹����� X���� ������ x,y 2d��ǥ�� �ϴ� ����Ѱ�
	 */
	FVector revCam = FRotator(0.0f, owner->GetControlRotation().Yaw + 180.0f, 0.0f).RotateVector(xOneVec);
	float camYawAtWallNorm = CMath::AngleBetweenVec(wallNorProjectGround, revCam);//camYawAtWallNorm = wallDistCamDegree

	tempVec = FVector::CrossProduct(revCam, wallNorProjectGround);
	if (0.0f <= tempVec.Z) ///z�� ������ ��ȣ�� plus, ����� ��ȣ�� min
	{
		camYawAtWallNorm *= -1.0f;
	}
	forwardVec = FRotator(0.0f, camYawAtWallNorm, 0.0f).RotateVector(FVector(1.0f, 0.0f, 0.0f));

	
	/** ||������� ����ִ� ���� Vertical���⸦ �ݿ�||
	 *	1. ���� ���� ������ ���Ѵ�.
	 *	2. ���� ������ pitch�� ȸ����ų�ǵ� ��Ʈ�ѷ��� ���� �ݴ����̸� ��,�Ʒ��� ���������� ������ �׶� 180���� ���Ѵ�.
	 */
	float wallLean = CMath::AngleBetweenVec(upVecProjectWall, revWallNormProjectGround);
	if (FVector::DotProduct(revCam, wallNorProjectGround) < backBoundaryValue)
	{///��Ʈ�ѷ��� ������� ��normal���� �������� ������ �ݴ���
		wallLean += 180.0f;
	}
	forwardVec = FRotator(wallLean, 0.0f, 0.0f).RotateVector(forwardVec);


	/** ||������� ���� Yaw��ŭ ������ ���||
	 *	1. ���� Normal�� X�࿡�� ��ŭ�� Yaw�� ȸ���Ǿ� �ִ��� ���Ѵ�. (��ȣ�� ���� ���� ������)
	 *	2. ��ȣ�� ������ ���� ���Ѵ�.
	 *	3. �׸�ŭ ������� Yawȸ���� ���ش�.
	 */
	float wallYaw = CMath::AngleBetweenVec(xOneVec, revWallNormProjectGround);
	tempVec = FVector::CrossProduct(xOneVec, wallNorProjectGround);
	if (0.0f <= tempVec.Z)
	{///�������� Z�� ����� ��ȣ�� min
		wallYaw *= -1.0f;
	}
	forwardVec = FRotator(0.0f, wallYaw, 0.0f).RotateVector(forwardVec);


	/** ||UpVector, RightVector�� ���Ѵ�.||
	 *	1. �������� ���ϴµ� ���� �ݴ������� �¿찡 �ٲ�� ������ UpVector�� ��Normal�� �����ͷ� �����־� ���Ѵ�.
	 */
	upVec = finalWall.ImpactNormal;
	if (FVector::DotProduct(revCam, wallNorProjectGround) < backBoundaryValue)
	{///��Ʈ�ѷ��� ������� ��normal���� �������� ������ �ݴ���
		upVec *= -1.0f;;
	}
	rightVec = FVector::CrossProduct(upVec, forwardVec);

	this->projectWallAxes = UKismetMathLibrary::MakeRotationFromAxes(forwardVec.GetSafeNormal(), rightVec.GetSafeNormal(), upVec.GetSafeNormal());
}

void UCWallRunComponent::UpdateRotation()
{
	/** ||�̸� ���س��� �����ʺ��Ϳ� velocity�� ������ ��Normal*-1 ���⿡ Yaw�� ȸ������ ĳ���͸� ȸ���� ����.||
	 * 1. ��Normal * -1�� ���� Yaw������ ���Ѵ�.(tempYaw)
	 * 2. tempYaw�� ���ؿ��� movingDegree��(-90 ~ 90)�� �����־� ������ Set���ش�.
	 */
	//TODO : pitchȸ���� �ؾ���
	float tempYaw = UKismetMathLibrary::MakeRotFromX((finalWall.ImpactNormal * -1)).Yaw;
	FRotator resultRot = FRotator(0.0f, tempYaw + movingDegree, 0.0f);
	owner->SetActorRotation(resultRot);
}

void UCWallRunComponent::UpdateLocation()
{
	/** ||����� ���� ���ų�, ������ÿ� Ʈ���̽��� ���Ǹ� �������� �ʽ��ϴ�. || */
	if (finalWall.bBlockingHit == false || bLastHitTrace == true) return;

	float horizontalOffset = 72.0f;
	float veticalOffset = 72.0f;
	float offSet = 0.0f;
	FVector tempVec = FVector::ZeroVector;

	/** ||WallRun Anim�� �߰� ���� ���̸� �ִ��� �ڿ������� �ϱ����� Location Set||
	 * 1. ���ʰ� �������� �־����� ���� ���� ���� ������ ���밪(Abs)�� �����־� 0 ~ 90 ���������� ����ȭ ���� => ��� ���� 0 ~ 1
	 * 2. �� �ڿ������� offset�� Lerp�Ͽ� ���Ѵ�.
	 * 3. ������ ��Normal�������� offset�� ���Ͽ� ��ġ ����
	 */
	offSet = UKismetMathLibrary::NormalizeToRange(UKismetMathLibrary::Abs(movingDegree), 0.0f, 90.0f);
	offSet = UKismetMathLibrary::Lerp(horizontalOffset, veticalOffset, offSet);
	tempVec = FMath::VInterpTo(owner->GetActorLocation(), finalWall.ImpactPoint + (finalWall.ImpactNormal * offSet), deltaTime, interpSpeed);
	owner->SetActorLocation(tempVec);
}

void UCWallRunComponent::UpdateMovingDegree()
{
	/** ||������ ���� ���Ϳ� ĳ������ Velocity������ ������ ���Ѵ�.||
	 * 1. ������ ���� �ö󰡴� ������ ���͸� ���Ѵ�.
	 * 2. velocity�� ������ ArcCos�� Degree������ ���Ѵ�.(��ȣ�� ���� ������ ������!)
	 * 3. WallRun�ִ� ���� �����̽��� ������ -90 ~ 90 ���� ������ �ؿ��� ���� ��ȣ�� ���ϱ� ������ 0 ~ 90���� Clamp
	 */
	FVector velocity = owner->GetVelocity();
	velocity = velocity.GetSafeNormal();


	float dotResult = FVector::DotProduct(velocity, upVecProjectWall);
	this->movingDegree = UKismetMathLibrary::DegAcos(dotResult);
	this->movingDegree = UKismetMathLibrary::ClampAngle(this->movingDegree, 0.0f, 90.0f);

	
	/** ||������ ���� Velocity������ ���� ���� ������ ���ʿ� �ִ��� �����ʿ� �ִ��� ��ȣ�� ����||
	 * 1. �������� �������� ���Ͽ� ���� Normal���Ϳ� �����Ͽ� ��ȣ �Ǻ�
	 */
	float sign = FVector::DotProduct(finalWall.ImpactNormal, FVector::CrossProduct(upVecProjectWall, velocity));

	/** ||���������� ��ȣ�� ���Ͽ� ���� ������ ����|| */
	if (0.10f <= sign)  return;
	this->movingDegree *= -1; return;
}

void UCWallRunComponent::StartBoost()
{
	/** ||���� ����ϴ� ����||
	 *	wallNormProjecGround	: ���� ����� �ٴڿ� ������Ų ����
	 *	revWallNormProjecGround	: ���� �� ����
	 *	finalDir				: ���� �ν�Ʈ�� ����
	 *	xOneVec					: FVector(1, 0, 0)
	 *	wallUpVecProject		: FVector(0, 0, 1)�� ���� ������Ų ����
	 */
	FVector wallNormProjecGround = FVector::VectorPlaneProject(finalWall.ImpactNormal, FVector(0.0f, 0.0f, 1.0f));
	FVector revWallNormProjecGround = wallNormProjecGround * -1.0f;
	FVector finalDir = FVector::ZeroVector;
	FVector xOneVec = FVector(1.0f, 0.0f, 0.0f);
	FVector wallUpVecProject = FVector::VectorPlaneProject(FVector(0.0f, 0.0f, 1.0f), finalWall.ImpactNormal);


	/** ||ĳ������ ����� ���� ����ֺ����� ������ ���Ͽ� X���� �������� �����Ѵ�.|| */
	float tempYaw = CMath::SignAngleBetweenVec(revWallNormProjecGround, FVector(0.0f, 0.0f, 1.0f), owner->GetActorForwardVector());
	finalDir = FRotator(0.0f, tempYaw, 0.0f).RotateVector(FVector(1.0f, 0.0f, 0.0f));



	/** ||���� Vertical���⸦ �ݿ�|| */
	float wallLean = CMath::AngleBetweenVec(wallUpVecProject, revWallNormProjecGround);
	finalDir = FRotator(wallLean, 0.0f, 0.0f).RotateVector(finalDir);


	/** ||���� Yaw��ŭ ������ ����� �ν�Ʈ ����||
	 *	1. ���� �� Normal�� X�࿡�� ��ŭ�� Yaw�� ȸ���Ǿ� �ִ��� ���Ѵ�. (��ȣ�� ���� ���� ������)
	 *	2. ��ȣ�� ������ ���� ���Ѵ�.
	 *	3. �׸�ŭ ������� Yawȸ���� ���ش�.
	 *	4. ������ �������� �ν�Ʈ ����
	 */
	float wallYaw = CMath::AngleBetweenVec(xOneVec, revWallNormProjecGround);
	FVector tempVec = FVector::CrossProduct(xOneVec, revWallNormProjecGround);
	if ( tempVec.Z < 0.0f)
	{///�������� Z�� ����� ��ȣ�� plus
		wallYaw *= -1.0f;
	}
	finalDir = FRotator(0.0f, wallYaw, 0.0f).RotateVector(finalDir);
	finalDir.Z += 0.1f;

	owner->GetCharacterMovement()->AddImpulse(finalDir*130000.0f);
}

void UCWallRunComponent::StartWallRun()
{
	StartBoost();
	
	if (OnWallStart.IsBound() == true) { OnWallStart.Execute(); }
	owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	owner->GetCharacterStruct()->SetSubState_Wallrun();
	eWallRunState = EWallRunState::DOING;
}

void UCWallRunComponent::EndWallRun()
{
	finalWall.Init();
	startBoost = false;
	eWallRunState = EWallRunState::IMPOSSIBLE;
	owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	owner->GetCharacterStruct()->SetSubState_None();
}

void UCWallRunComponent::WallJump()
{
	if (OnWallJump.IsBound() == true)
	{
		owner->GetCharacterStruct()->SetMainState_Air();
		OnWallJump.Execute();
	}
	
	if (movingDegree < -20.0f)
	{
		owner->PlayAnimMontage(leftWallJump);
	}
	else if (20.0f < movingDegree)
	{
		owner->PlayAnimMontage(rightWallJump);
	}
	else
	{
		owner->PlayAnimMontage(verticalWallJump);
	}

	owner->GetCharacterMovement()->AddImpulse(
		owner->GetVelocity() * wallJumpMulVel + 
		finalWall.ImpactNormal *wallJumpMulNorm + 
		upVecProjectWall * wallJumpMulUp
	);
}

FVector UCWallRunComponent::GetFinalDir(bool keyboardDir, float axisValue)
{
	/** ||�÷��̾ ���� ����Ű�� ������� ��������� �̵����� ������ ���մϴ�.|| */
	FVector finalVec = FVector::ZeroVector;
	if (keyboardDir == true)
	{
		finalVec = UKismetMathLibrary::GetForwardVector(projectWallAxes);
	}
	else
	{
		finalVec = UKismetMathLibrary::GetRightVector(projectWallAxes);
	}
	//GetInputAxisValue
	finalVec *= axisValue;
	//180�� ȸ�������� �־���
	finalVec += upVecProjectWall * 0.1f;
	
	return finalVec;
}

void UCWallRunComponent::Debug()
{

	//Debug WallRunState 
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange,
			FString::Printf(TEXT("EWallRunState : %s"), *UEnum::GetDisplayValueAsText(eWallRunState).ToString())
		);
	}

	////Debug movingDegree
	//{
	//	CLog::ScreenLog(CMath::AngleBetweenVec(upVecProjectWall, owner->GetVelocity()), 0.0f, FColor::Orange);
	//}

	//Debug ������ ĸ�� Ʈ���̽� , �ʷϻ�
	{
		FColor tempColor = FColor::Red;
		TArray<FHitResult> tempHits;

		if (GetWorld()->SweepMultiByChannel(tempHits, owner->GetActorLocation(),
			owner->GetActorLocation() + FVector(0, 0, 1), FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeCapsule(radius, halfHeight)) == false)
		{
			tempColor = FColor::Green;
		}

		DrawDebugCapsule(GetWorld(),
			owner->GetActorLocation(),
			halfHeight, radius, FQuat::Identity, tempColor);
	}

	//Debug ���� ����ִ� �κ� �簢�� Ʈ���̽�, ������
	{
		if (finalWall.bBlockingHit == true)
			DrawDebugBox(GetWorld(), finalWall.ImpactPoint, FVector(10, 10, 10), FColor::Red, false, -1.0f, 0, 4);
	}

	//���� forward, right, up ����, ȸ��
	{
		
		FVector forwardVec = UKismetMathLibrary::GetForwardVector(projectWallAxes);
		FVector upVec = UKismetMathLibrary::GetUpVector(projectWallAxes);
		FVector rightVec = UKismetMathLibrary::GetRightVector(projectWallAxes);

		//forward - Red
		DrawDebugDirectionalArrow(GetWorld(), owner->GetActorLocation(),
			(owner->GetActorLocation() + forwardVec * arrowLength),
			arrowSize, FColor::Red, false, -1.0f, 0, arrowThick);

		//right - red
		DrawDebugDirectionalArrow(GetWorld(), owner->GetActorLocation(),
			(owner->GetActorLocation() + rightVec * arrowLength),
			arrowSize, FColor::Blue, false, -1.0f, 0, arrowThick);

		//up - blue
		DrawDebugDirectionalArrow(GetWorld(), owner->GetActorLocation(),
			(owner->GetActorLocation() + upVec * arrowLength),
			arrowSize, FColor::Silver, false, -1.0f, 0, arrowThick);
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//DrawDebugDirectionalArrow(GetWorld(), owner->GetActorLocation(),
//(ownerLoc + oppositeWallBoostDir * arrowLength),
//arrowSize, FColor::Red, false, 7.0f, 0, arrowThick);
//
//DrawDebugBox(GetWorld(), cornerLoc, FVector(10, 10, 10), FColor::Red, false, 7.0f, 0, 4);
//DrawDebugBox(GetWorld(), finalWall.ImpactPoint, FVector(10, 10, 10), FColor::Blue, false, 7.0f, 0, 4);
//
//CLog::ScreenLog(calBeforeDegree, 7.0f, FColor::Red, L"calBeforeDegree : ");
////////////////////////////////////////////////////////////////////////////////////////////////////
