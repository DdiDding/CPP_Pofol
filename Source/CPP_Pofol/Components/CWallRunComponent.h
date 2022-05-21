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
 * WallRun�� �ʿ��� �����͸� Update�ϰ� �����ü��ִ�.
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

	//Wall jump�� �ϱ� ���� anim inst�� ������ ��������Ʈ
	FwallJump OnWallJump;
	//���� �� ��Ÿ�⸦ �ϸ� �������°� �Ǿ ���ֱ� ���� anim inst�� ���� ��������Ʈ
	FwallJump OnWallStart;

//////////////////////////////////////////////////////////////////////////
public://�÷��̾�� ȣ���ϴ� �Լ�

	/**
	 * Space�� ������ ȣ�� �Ǵ� �Լ�
	 * wall run�� �������� Ȯ���Ͽ� �����ϴٸ� bIsDoingWallRun�� true�� �ϰ� true�� ��ȯ�մϴ�.
	 * �������� �ʴٸ� false�� ��ȯ�մϴ�.
	 */
	virtual bool StartSubState() override;

	void UpdateRotation();

	void UpdateLocation();

	/**
	 * Ű����� ���� ������ �����Ͽ� �ƿ� ������ ������ �� ���� ������ ������ �÷��̾ ���� �� ���� ������ ��ȯ�մϴ�.
	 * @param keyboardDir - True�� Forward���� ȣ��ǰ�, False�� Side���� ȣ��˴ϴ�.
	 * @param axisValue - �÷��̾��� ���� axisValue
	 * @return - �÷��̾ �̵��� ���� ���⺤�� ----�̰� ���Եɼ������� 
	 */
	FVector GetFinalDir(bool keyboardDir, float axisValue);

//////////////////////////////////////////////////////////////////////////
public:// Getter, Setter

	UFUNCTION(BlueprintPure)
	const float GetMovingDegree() { return movingDegree; }

	UFUNCTION(BlueprintPure)
	const FRotator GetProjectWallAxes() { return projectWallAxes; }


//////////////////////////////////////////////////////////////////////////
private://Update, Tick���� ����˴ϴ�.

	/**WallRun�߿� ������Ʈ �ؾ��Ұ͵�*/
	void UpdateDoingWallRun();
	
			/*** ������ ������ ������Ʈ*/
			void UpdateProperty();

			/*** projectWallAxes�� ������Ʈ �մϴ�. */
			void UpdateProjectWallAxes();

			/*** wallrun �ִϸ��̼� BlendSpace�� ���� ���� ����մϴ�. movingDegree�� ���� ����˴ϴ�. */
			void UpdateMovingDegree();


			/*** ��ó�� WallRun�� ������ ���� �ִ��� Ȯ���մϴ�. �ִٸ� True, ������ False*/
			bool UpdateIsPossibleWallRunState();

					/**** �ٴڿ� ����ִ��� Ȯ���մϴ�.
					 * @Return : True�� ���� ����ְ� False�� ���� ���� �ʽ��ϴ�.
					 */
					bool UpdateIsOnGround();

					/**** ���� ������ ���� ã�� hitWallResult�� �־��ݴϴ�.*/
					bool UpdateFinalWall();

private:
	EWallRunState eWallRunState{ EWallRunState::IMPOSSIBLE };

	//���� ���� ����� ĳ������ Velocity�� ���� ����
	float movingDegree;

	//W�� ������ �̵��� ������ forward, D�� ������ �̵��� ������ right�� ���� custom axes�� ����˴ϴ�.
	FRotator projectWallAxes;
	

//////////////////////////////////////////////////////////////////////////
private://member Function & Meber Property

	void StartWallRun();

	/**���۽ÿ� �ν�Ʈ�ִ� ���*/
	void StartBoost();

	/**WallRun ������ �����Ҷ�*/
	void EndWallRun();

	/**WallRun�߿� �����ϴ� ���*/
	void WallJump();

private:

	/**Owner Object�� �����͸� �������� �ʱ⿡ ���⿡ ������ ������ ���ϰ� ���*/
	ACPlayer * owner;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * leftWallJump;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * rightWallJump;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * verticalWallJump;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage * wallExit;

	/**wallrun�� ���� ������ ���� HitResult�� ��� ����, wall run�߿� ������Ʈ�ȴ�.*/
	FHitResult finalWall;

	//FVector(0, 0, 1) �� ���� ������Ų ����
	FVector upVecProjectWall;

	float deltaTime{ 0.0f };

	//ĳ������ �ּ� ���ǵ�, �� ���ǵ庸�� �پ��� WallRun����
	const float minimumOwnerSpeed{ 270.0f };

	/**Sphere Trace �Ӽ�*/
	const float radius{ 200.0f };
	const float halfRadius{ 100.0f };
	const float halfHeight{ 80.0f };

	UPROPERTY(EditAnywhere, Category = "Debug")
	float interpSpeed{ 0.25 };

	//Update�߿� ���۽� �ν�Ʈ�� �ѹ��� �־���ϱ⶧���� �÷��� ���� ����
	bool startBoost{ false };
	
	//UpdateFinalWall���� ���� ƽ���� Ʈ���̽��� Hit�� �Ǿ����� ����, Hit�Ǿ����� True
	bool bLastHitTrace{ false };
	//���鿡 ���� ��ŭ�� ������ ������ �ν�Ʈ�� �����Ұ����� �ִ� ���� ��
	const float maxTraceDegree{ 110.0f };

	//Ʈ���̽��� ���� �� �ڳ��� ��ġ
	FVector cornerLoc{ 0.0f,0.0f,0.0f };
	//�𼭸����� ù����ImpactPoint������ ����
	FVector calBeforeStartDir{ 0.0f,0.0f,0.0f };
	//calBeforeStartDir�� ���� �� �������� ����
	float calBeforeDegree{ 0.0f };

	//���Ŀ� ���� �� Ʈ���̽�����
	FVector calAfterStartDir{ 0.0f,0.0f,0.0f }; //�̰� �������� finalWall�� Normal�� �ȴ�.
	FVector calAfterEndDir{ 0.0f,0.0f,0.0f }; 

	//WallJum�Ҷ� ���󰡰� �ϴ� �� �������� ����
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
