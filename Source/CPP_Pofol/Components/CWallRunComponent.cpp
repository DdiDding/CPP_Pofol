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
	
	//EWallRunState::DOING일때의 업데이트
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
	//TODO : 나중에 이걸 Character로 빼서 여기서 MainSTate가 Ground인지 확인해야할수도있음
	FFindFloorResult result;
	owner->GetCharacterMovement()->FindFloor(owner->GetActorLocation(), result, false);

	if (FMath::IsNearlyZero(result.FloorDist, 0.05f)) { return true; }
	else return false;
}

bool UCWallRunComponent::UpdateFinalWall()
{
	/**	||벽을 검출하기 위한 트레이스를 사용합니다.|| 
	 *	ownerLoc	: 캐릭터의 현재 위치
	 *	tempHits		: WallRun채널을 쏘아 1차적으로 모든 벽타기가 가능한 벽을 검출합니다.
	 *	arrayWallInRange: 2차적으로 tempHits에서 캐릭터 전방 180도 안에 있는 벽을 검출해 저장합니다.
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
		{///낭떠러지쪽에 있는 벽은 검출하지 않습니다.
			continue;
		}
		if (FVector::DotProduct(owner->GetActorForwardVector().GetSafeNormal(), tempHits[i].ImpactNormal) <= 0.2f)
		{///전방 180도 안에 들어오면 이 영역을 실행합니다.
			arrayWallInRange.Emplace(tempHits[i]);
		}
	}


	if (arrayWallInRange.Num() == 0)
	{
		finalWall.bBlockingHit = false;
		return false;
	}
	
	
	/**	||최종적으로 가장 가까운 벽을 찾습니다.||
	 *	minLength	: 가장 가까운 거리를 업데이트 하기 위한 값, 1차 검출 반경이 radius이므로 기본값으로 설정.
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


	/**	||플레이어의 다리쪽에서 쏘아 검출이 안되면 상반신만 붙어 있는 것 이므로 가능한 벽으로 치지 않습니다.||
	 *	
	 */
	FHitResult tempHit;
	FVector startLoc = FVector(ownerLoc.X, ownerLoc.Y, ownerLoc.Z - 44.0f);

	if (GetWorld()->LineTraceSingleByChannel(tempHit,
		startLoc, startLoc + (finalWall.ImpactNormal * -1.0f) * radius,
		ECollisionChannel::ECC_GameTraceChannel1) == false)
	{///트레이스에 아무것도 맞지 않으면 이 영역 실행
		return false;
	}


	/**	||플레이어의 진행방향으로 막혀있는 벽이 있는지 트레이스를 사용해 확인합니다.|| 
	 *	트레이스에서 검출이 되는 시점에 실행되는 부분과 그 다음 틱부터에서 쭉 실행되는 부분으로 나누어져있습니다.
	 *	처음 검출 시점에 실행되는 부분에서는 벽이 있는지, 그 다음 트레이스에 실행될때 필요한 데이터를 저장 합니다.
	 *	
	 *	lineHit				: 모서리의 위치 벡터
	 *	lineTraceDir		: 트레이스가 쏘아질 방향
	 *	oppositeWallBoostDir: 모서리를 돌아 나오는 벽으로 이동하기위한 부스트 방향
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
		{///트레이스에 아무것도 맞지 않으면 이 영역 실행
			return true;
		}
		
		cornerLoc = lineHit.ImpactPoint;
		oppositeWallBoostDir = FVector::VectorPlaneProject(finalWall.ImpactNormal, lineHit.ImpactNormal).GetSafeNormal2D();

		//밑을 위한 변수 저장
		calBeforeStartDir = (ownerLoc - cornerLoc).GetSafeNormal2D();
		calBeforeDegree = CMath::AngleBetweenVec(calBeforeStartDir, oppositeWallBoostDir);

		//일정 각 만 트레이스진행하고 나머지 그냥 하기
		if (maxTraceDegree < calBeforeDegree)
		{
			return true;
		}

		calAfterStartDir = finalWall.ImpactNormal * -1.0f;
		calAfterEndDir = lineHit.ImpactNormal * -1.0f;
		

		/**	||막힌 벽으로 부드럽게 가기 위한 부스트를 줍니다.||
		 *	boostDir	: 부스트 방향벡터를 구할때 의 도착 벡터
		 *	boostSpeed	: 부스트 스피드
		 */
		FVector boostDir = (cornerLoc + (oppositeWallBoostDir * radius * 3.0f) - ownerLoc).GetSafeNormal2D();
		//TODO : 각도에 대한 영향, Velocity에 대한 영향
		const float boostSpeed =
			50000.0f///기본 값
			+ (2000.0f / calBeforeDegree)///각도에 대한 영향
			+ (owner->GetVelocity().Size() * 40.0f);///캐릭터 벨로시티에 대한 영향
		owner->GetCharacterMovement()->AddImpulse(boostDir * boostSpeed);

		bLastHitTrace = true;
	}
	if (bLastHitTrace == true)
	{
		/**	||벽 사이의 각도, 벽의 노멀끼리의 각도를 보간 값을 이용하여 Impact Normal을 설정합니다.||
		*	1.현재 캐릭터위치가 벽 사이의 각도에서 얼마만큼의 각도를 가진지 구하고 정규화한 값을 구합니다.
		*	2.그 값을 통해 벽의 노멀끼리의 각도에서 보간 값을 구합니다. 그 값이 Impact Normal값이 됩니다.
		*	
		*	tempCorner : 위에 저장한 Corner는 Z값이 업데이트가 안되서 새로 업데이트 해준 Corner값
		*/
		FVector tempCorner = FVector(cornerLoc.X, cornerLoc.Y, ownerLoc.Z);
		lineTraceDir = (tempCorner - ownerLoc).GetSafeNormal2D();

		if (GetWorld()->LineTraceSingleByChannel(lineHit,
			ownerLoc, ownerLoc + lineTraceDir * radius,
			ECollisionChannel::ECC_GameTraceChannel1) == false)
		{///트레이스에 아무것도 맞지 않으면 이 영역 실행
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

	/** ||WallRun중에 땅에 붙었는지 확인하고 캐릭터의 속도가 일정 속도 이하면 WallRun에서 종료됩니다.|| */
	if (UpdateIsOnGround() == true || owner->GetVelocity().Size() < minimumOwnerSpeed)
	{
		owner->GetCharacterStruct()->SetMainState_Ground();
		EndWallRun();
		return false;
	}
	

	/** ||벽타기중에 옥상에 다다랐을 때 알맞은 처리를 하고 WallRUn을 끝냅니다.||
	 *	1.검출중인 벽이 upVec과 내적이 0.5이상이면 옥상으로 올라간다로 간주하여 처리합니다.
	 *	2.벽으로 넘어 가는 방향으로 캐릭터를 밀어냅니다. 캐릭터의 Velocity도 영향을 줍니다.
	 *	3.넘어가는 몽타주 재생합니다.
	 */
	if (0.65f <= FVector::DotProduct(finalWall.ImpactNormal, FVector(0.0f, 0.0f, 1.0f)))
	{
		FVector tempDir = FVector::VectorPlaneProject(finalWall.ImpactNormal * -1.0f, FVector(0.0f, 0.0f, 1.0f)).GetSafeNormal();
		FVector boostDir = owner->GetVelocity().GetSafeNormal2D();

		boostDir += tempDir * 0.7f; ///올라가는 지면 방향으로 0.7만큼 벡터를 더해준다.
		boostDir += FVector(0.0f, 0.0f, 0.6f); ///위로 0.8만큼 벡터를 더해준다
		owner->GetCharacterMovement()->AddImpulse(boostDir*130000.0f);
		
		owner->PlayAnimMontage(wallExit);
		EndWallRun();
		return false;
	}


	//마지막에 벽검사와 동시에 벽 검출해 저장
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
		//키누르면 벽점프
		//키안누르고 스페이스바면 제자리 떨어지기
		return true;
	}

	//가능한 벽이 있는지 확인한다.
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
	/**	||두번 이상 쓰게 되는 변수||
	 *	wallNorProjectGround		: 벽Normal을 땅바닥에 투영시킨 벡터
	 *	revWallNormProjectGround	: 위(wallNorProjectGround)의 역벡터
	 *	xOneVec						: x축에 정면인 벡터
	 *	tempVec						: 잠시 담아두는 벡터
	 *	forwardVec					: W를 눌렀을 때 이동하려는 방향을 가진 벡터
	 *	upVec						: 벽의 Normal, 혹은 그 역벡터
	 *	rightVec					: D를 눌렀을 때 이동하려는 방향을 가진 벡터
	 *	backBoundaryValue			: 컨트롤러가 벽 뒷편으로가면 값이 뒤집어지는데 이걸 내적으로 판별하는데 0을 기준으로 하면
	 *								  부자연스러워서 자연스럽게 하기 위한 내가 정한 뒷편이다 하는 내적 경계 값
	 */
	FVector wallNorProjectGround = FVector::VectorPlaneProject(finalWall.ImpactNormal, FVector(0.0f, 0.0f, 1.0f));
	FVector revWallNormProjectGround = wallNorProjectGround * -1;
	FVector xOneVec = FVector(1.0f, 0.0f, 0.0f);
	FVector tempVec = FVector::ZeroVector;
	FVector forwardVec = FVector::ZeroVector;
	FVector upVec = FVector::ZeroVector;
	FVector rightVec = FVector::ZeroVector;
	float backBoundaryValue = -0.8f;


	/** ||벽의 Normal에서 컨트롤러의 역벡터가 얼만큼의 각도를 가지고있는지, 그 각도를 X축정면에서 회전시킨 벡터를 구합니다.||
	 *	1. 컨트롤러의 역방향 벡터를 구한다. = 벽의 hitLocation에서 컨트롤러쪽으로 향하는 벡터
	 *	2. 벽의 Normal에서 1번의 벡터를 이용해 그 사이각인 부호가 없는 절대 각도를 구한다.
	 *	3. 외적을 이용해서 부호를 구하여 X축을 향한 벡터를 돌려준다. = 입력방향을 X축이 정면인 x,y 2d좌표로 일단 계산한것
	 */
	FVector revCam = FRotator(0.0f, owner->GetControlRotation().Yaw + 180.0f, 0.0f).RotateVector(xOneVec);
	float camYawAtWallNorm = CMath::AngleBetweenVec(wallNorProjectGround, revCam);//camYawAtWallNorm = wallDistCamDegree

	tempVec = FVector::CrossProduct(revCam, wallNorProjectGround);
	if (0.0f <= tempVec.Z) ///z가 음수면 부호가 plus, 양수면 부호가 min
	{
		camYawAtWallNorm *= -1.0f;
	}
	forwardVec = FRotator(0.0f, camYawAtWallNorm, 0.0f).RotateVector(FVector(1.0f, 0.0f, 0.0f));

	
	/** ||결과물에 닿고있는 벽의 Vertical기울기를 반영||
	 *	1. 벽의 기울기 각도를 구한다.
	 *	2. 구한 각도로 pitch를 회전시킬건데 컨트롤러가 벽의 반대편이면 위,아래가 뒤집어지기 때문에 그때 180도를 더한다.
	 */
	float wallLean = CMath::AngleBetweenVec(upVecProjectWall, revWallNormProjectGround);
	if (FVector::DotProduct(revCam, wallNorProjectGround) < backBoundaryValue)
	{///컨트롤러의 역방향과 벽normal값의 내적값이 음수면 반대편
		wallLean += 180.0f;
	}
	forwardVec = FRotator(wallLean, 0.0f, 0.0f).RotateVector(forwardVec);


	/** ||결과물을 벽의 Yaw만큼 돌리는 계산||
	 *	1. 벽의 Normal이 X축에서 얼만큼의 Yaw로 회전되어 있는지 구한다. (부호가 없는 절대 각도임)
	 *	2. 부호를 외적을 통해 구한다.
	 *	3. 그만큼 결과물에 Yaw회전을 해준다.
	 */
	float wallYaw = CMath::AngleBetweenVec(xOneVec, revWallNormProjectGround);
	tempVec = FVector::CrossProduct(xOneVec, wallNorProjectGround);
	if (0.0f <= tempVec.Z)
	{///외적값의 Z가 양수면 부호는 min
		wallYaw *= -1.0f;
	}
	forwardVec = FRotator(0.0f, wallYaw, 0.0f).RotateVector(forwardVec);


	/** ||UpVector, RightVector를 구한다.||
	 *	1. 외적으로 구하는데 벽의 반대편에서는 좌우가 바뀌기 때문에 UpVector를 벽Normal의 역벡터로 취해주어 구한다.
	 */
	upVec = finalWall.ImpactNormal;
	if (FVector::DotProduct(revCam, wallNorProjectGround) < backBoundaryValue)
	{///컨트롤러의 역방향과 벽normal값의 내적값이 음수면 반대편
		upVec *= -1.0f;;
	}
	rightVec = FVector::CrossProduct(upVec, forwardVec);

	this->projectWallAxes = UKismetMathLibrary::MakeRotationFromAxes(forwardVec.GetSafeNormal(), rightVec.GetSafeNormal(), upVec.GetSafeNormal());
}

void UCWallRunComponent::UpdateRotation()
{
	/** ||미리 구해놓은 벽위쪽벡터와 velocity의 각도를 벽Normal*-1 방향에 Yaw를 회전시켜 캐릭터를 회전을 세팅.||
	 * 1. 벽Normal * -1의 절대 Yaw각도를 구한다.(tempYaw)
	 * 2. tempYaw의 기준에서 movingDegree값(-90 ~ 90)을 더해주어 각도를 Set해준다.
	 */
	//TODO : pitch회전도 해야함
	float tempYaw = UKismetMathLibrary::MakeRotFromX((finalWall.ImpactNormal * -1)).Yaw;
	FRotator resultRot = FRotator(0.0f, tempYaw + movingDegree, 0.0f);
	owner->SetActorRotation(resultRot);
}

void UCWallRunComponent::UpdateLocation()
{
	/** ||검출된 벽이 없거나, 벽검출시에 트레이스가 사용되면 보간하지 않습니다. || */
	if (finalWall.bBlockingHit == false || bLastHitTrace == true) return;

	float horizontalOffset = 72.0f;
	float veticalOffset = 72.0f;
	float offSet = 0.0f;
	FVector tempVec = FVector::ZeroVector;

	/** ||WallRun Anim의 발과 벽의 사이를 최대한 자연스럽게 하기위한 Location Set||
	 * 1. 왼쪽과 오른쪽의 멀어지는 사이 값은 같이 때문에 절대값(Abs)를 취해주어 0 ~ 90 영역에서의 정규화 진행 => 기대 값은 0 ~ 1
	 * 2. 각 자연스러운 offset을 Lerp하여 구한다.
	 * 3. 벽에서 벽Normal방향으로 offset을 구하여 위치 설정
	 */
	offSet = UKismetMathLibrary::NormalizeToRange(UKismetMathLibrary::Abs(movingDegree), 0.0f, 90.0f);
	offSet = UKismetMathLibrary::Lerp(horizontalOffset, veticalOffset, offSet);
	tempVec = FMath::VInterpTo(owner->GetActorLocation(), finalWall.ImpactPoint + (finalWall.ImpactNormal * offSet), deltaTime, interpSpeed);
	owner->SetActorLocation(tempVec);
}

void UCWallRunComponent::UpdateMovingDegree()
{
	/** ||벽면의 위쪽 벡터와 캐릭터의 Velocity방향의 각도를 구한다.||
	 * 1. 벽면을 따라 올라가는 방향의 벡터를 구한다.
	 * 2. velocity와 내적후 ArcCos로 Degree각도를 구한다.(부호가 없는 각도로 구해짐!)
	 * 3. WallRun애님 블랜드 스페이스의 각도는 -90 ~ 90 까지 있지만 밑에서 따로 부호를 곱하기 때문에 0 ~ 90으로 Clamp
	 */
	FVector velocity = owner->GetVelocity();
	velocity = velocity.GetSafeNormal();


	float dotResult = FVector::DotProduct(velocity, upVecProjectWall);
	this->movingDegree = UKismetMathLibrary::DegAcos(dotResult);
	this->movingDegree = UKismetMathLibrary::ClampAngle(this->movingDegree, 0.0f, 90.0f);

	
	/** ||외적을 통해 Velocity방향이 벽의 위쪽 벡터의 왼쪽에 있는지 오른쪽에 있는지 부호를 구함||
	 * 1. 외적으로 수직벡터 구하여 벽의 Normal벡터와 내적하여 부호 판별
	 */
	float sign = FVector::DotProduct(finalWall.ImpactNormal, FVector::CrossProduct(upVecProjectWall, velocity));

	/** ||마지막으로 부호를 곱하여 최종 각도를 구함|| */
	if (0.10f <= sign)  return;
	this->movingDegree *= -1; return;
}

void UCWallRunComponent::StartBoost()
{
	/** ||자주 사용하는 변수||
	 *	wallNormProjecGround	: 벽의 노멀을 바닥에 투영시킨 벡터
	 *	revWallNormProjecGround	: 그의 역 벡터
	 *	finalDir				: 최종 부스트할 방향
	 *	xOneVec					: FVector(1, 0, 0)
	 *	wallUpVecProject		: FVector(0, 0, 1)을 벽에 투영시킨 벡터
	 */
	FVector wallNormProjecGround = FVector::VectorPlaneProject(finalWall.ImpactNormal, FVector(0.0f, 0.0f, 1.0f));
	FVector revWallNormProjecGround = wallNormProjecGround * -1.0f;
	FVector finalDir = FVector::ZeroVector;
	FVector xOneVec = FVector(1.0f, 0.0f, 0.0f);
	FVector wallUpVecProject = FVector::VectorPlaneProject(FVector(0.0f, 0.0f, 1.0f), finalWall.ImpactNormal);


	/** ||캐릭터의 방향과 벽의 역노멀벡터의 각도를 구하여 X축을 기준으로 설정한다.|| */
	float tempYaw = CMath::SignAngleBetweenVec(revWallNormProjecGround, FVector(0.0f, 0.0f, 1.0f), owner->GetActorForwardVector());
	finalDir = FRotator(0.0f, tempYaw, 0.0f).RotateVector(FVector(1.0f, 0.0f, 0.0f));



	/** ||벽의 Vertical기울기를 반영|| */
	float wallLean = CMath::AngleBetweenVec(wallUpVecProject, revWallNormProjecGround);
	finalDir = FRotator(wallLean, 0.0f, 0.0f).RotateVector(finalDir);


	/** ||벽의 Yaw만큼 돌리는 계산후 부스트 적용||
	 *	1. 벽의 역 Normal이 X축에서 얼만큼의 Yaw로 회전되어 있는지 구한다. (부호가 없는 절대 각도임)
	 *	2. 부호를 외적을 통해 구한다.
	 *	3. 그만큼 결과물에 Yaw회전을 해준다.
	 *	4. 정해진 방향으로 부스트 적용
	 */
	float wallYaw = CMath::AngleBetweenVec(xOneVec, revWallNormProjecGround);
	FVector tempVec = FVector::CrossProduct(xOneVec, revWallNormProjecGround);
	if ( tempVec.Z < 0.0f)
	{///외적값의 Z가 양수면 부호는 plus
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
	/** ||플레이어가 누른 방향키를 기반으로 어느쪽으로 이동할지 방향을 구합니다.|| */
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
	//180도 회전때문에 넣어줌
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

	//Debug 벽검출 캡슐 트레이스 , 초록색
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

	//Debug 벽과 닿고있는 부분 사각형 트레이스, 빨간색
	{
		if (finalWall.bBlockingHit == true)
			DrawDebugBox(GetWorld(), finalWall.ImpactPoint, FVector(10, 10, 10), FColor::Red, false, -1.0f, 0, 4);
	}

	//벽의 forward, right, up 벡터, 회색
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
