#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/CAboutSubState.h"
#include "CWallRunComponent.generated.h"

#define DEBUGWALLRUN if(bDebugWallRun)

class ACPlayer;
class ACCharacter;
class UCurveFloat;

DECLARE_DELEGATE(FwallJump);

UENUM(BlueprintType)
enum class EWallRunState : uint8
{
	IMPOSSIBLE, POSSIBLE, DOING, SLIDING
};

/**
 * WallRun에 필요한 데이터를 Update하고 가져올수있다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_POFOL_API UCWallRunComponent : public UActorComponent, public ICAboutSubState
{
	GENERATED_BODY()

public:	
	UCWallRunComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Wall jump를 하기 위해 anim inst로 보내는 델리게이트
	FwallJump OnWallJump;
	//점프 후 벽타기를 하면 점프상태가 되어서 꺼주기 위한 anim inst로 가는 델리게이트
	FwallJump OnWallStart;

//////////////////////////////////////////////////////////////////////////
public://플레이어에서 호출하는 함수

	/**
	 * Space를 누르면 호출 되는 함수
	 * wall run이 가능한지 확인하여 가능하다면 bIsDoingWallRun을 true로 하고 true를 반환합니다.
	 * 가능하지 않다면 false를 반환합니다.
	 */
	virtual bool StartSubState() override;

	void UpdateRotation();

	void UpdateLocation();

	/**
	 * 키보드로 누른 방향을 제한하여 아에 밑으로 내려갈 수 없게 방향을 조절해 플레이어가 가야 할 최종 방향을 반환합니다.
	 * @param keyboardDir - True면 Forward에서 호출되고, False면 Side에서 호출됩니다.
	 * @param axisValue - 플레이어의 누른 axisValue
	 * @return - 플레이어가 이동할 최종 방향벡터 ----이거 쓰게될수도있음 
	 */
	FVector GetFinalDir(bool keyboardDir, float axisValue);

//////////////////////////////////////////////////////////////////////////
public:// Getter, Setter

	UFUNCTION(BlueprintPure)
	const float GetMovingDegree() { return movingDegree; }

	UFUNCTION(BlueprintPure)
	const FRotator GetProjectWallAxes() { return projectWallAxes; }


//////////////////////////////////////////////////////////////////////////
private://Update, Tick에서 실행됩니다.

	/**WallRun중에 업데이트 해야할것들*/
	void UpdateDoingWallRun();
	
			/*** 간단한 변수의 업데이트*/
			void UpdateProperty();

			/*** projectWallAxes를 업데이트 합니다. */
			void UpdateProjectWallAxes();

			/*** wallrun 애니메이션 BlendSpace를 위한 값을 계산합니다. movingDegree에 값이 저장됩니다. */
			void UpdateMovingDegree();


			/*** 근처에 WallRun이 가능한 벽이 있는지 확인합니다. 있다면 True, 없으면 False*/
			bool UpdateIsPossibleWallRunState();

					/**** 바닥에 닿아있는지 확인합니다.
					 * @Return : True면 땅에 닿아있고 False면 땅에 닿지 않습니다.
					 */
					bool UpdateIsOnGround();

					/**** 가장 적합한 벽을 찾아 hitWallResult에 넣어줍니다.*/
					bool UpdateFinalWall();

private:
	EWallRunState eWallRunState{ EWallRunState::IMPOSSIBLE };

	//벽의 위쪽 방향과 캐릭터의 Velocity의 사이 각도
	float movingDegree;

	//W를 누르면 이동할 방향을 forward, D를 누르면 이동할 방향을 right로 가진 custom axes가 저장됩니다.
	FRotator projectWallAxes;
	

//////////////////////////////////////////////////////////////////////////
private://member Function & Meber Property

	void StartWallRun();

	/**시작시에 부스트주는 기능*/
	void StartBoost();

	/**WallRun 끝낼때 정리할때*/
	void EndWallRun();

	/**WallRun중에 점프하는 기능*/
	void WallJump();

private:

	/**Owner Object의 데이터를 변경하지 않기에 여기에 변수로 저장해 편하게 사용*/
	ACPlayer * owner;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * leftWallJump;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * rightWallJump;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * verticalWallJump;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * wallExit;

	/**wallrun에 가장 적합한 벽의 HitResult를 담는 변수, wall run중에 업데이트된다.*/
	FHitResult finalWall;

	//FVector(0, 0, 1) 을 벽에 투영시킨 벡터
	FVector upVecProjectWall;

	float deltaTime{ 0.0f };

	//캐릭터의 최소 스피드, 이 스피드보다 줄어들면 WallRun종료
	const float minimumOwnerSpeed{ 270.0f };

	/**Sphere Trace 속성*/
	const float radius{ 200.0f };
	const float halfRadius{ 100.0f };
	const float halfHeight{ 80.0f };

	UPROPERTY(EditAnywhere, Category = "Debug")
	float interpSpeed{ 0.25 };

	//Update중에 시작시 부스트를 한번만 주어야하기때문에 플래그 변수 설정
	bool startBoost{ false };
	
	//UpdateFinalWall에서 이전 틱에서 트레이스에 Hit이 되었는지 저장, Hit되었으면 True
	bool bLastHitTrace{ false };
	//정면에 벽이 얼만큼의 각도를 가져야 부스트를 실행할건지의 최대 각도 값
	const float maxTraceDegree{ 110.0f };

	//트레이스로 검출 된 코너의 위치
	FVector cornerLoc{ 0.0f,0.0f,0.0f };
	//모서리에서 첫벽의ImpactPoint까지의 벡터
	FVector calBeforeStartDir{ 0.0f,0.0f,0.0f };
	//calBeforeStartDir와 검출 된 벽사이의 각도
	float calBeforeDegree{ 0.0f };

	//이후에 실행 될 트레이스에서
	FVector calAfterStartDir{ 0.0f,0.0f,0.0f }; //이걸 뒤집으면 finalWall의 Normal이 된다.
	FVector calAfterEndDir{ 0.0f,0.0f,0.0f }; 

	//WallJum할때 날라가게 하는 각 곱해지는 값들
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "WallRunJump", meta = (AllowPrivateAccess = true))
	float wallJumpMulVel{ 1.5f };
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "WallRunJump", meta = (AllowPrivateAccess = true))
	float wallJumpMulNorm{ 150000.0f };
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "WallRunJump", meta = (AllowPrivateAccess = true))
	float wallJumpMulUp{ 200000.0f };

//////////////////////////////////////////////////////////////////////////
private:// Debug

	void Debug();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool bDebugWallRun{ false };

private:

	//arrow debug value
	float arrowLength{ 400.0f };
	float arrowSize{ 2000.0f };
	float arrowThick{ 10.0f };
};
