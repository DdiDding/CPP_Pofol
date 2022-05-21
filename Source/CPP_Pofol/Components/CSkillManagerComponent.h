#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/CESkillType.h"
#include "CSkillManagerComponent.generated.h"

#define DEBUG_SKILLMANAGER if(IsDebugMode == true)

class ACCharacter;
class UCDA_Skill;
class UCDA_WeaponData;
class UCGameInstance;
class UCDamageType_Range;


DECLARE_DELEGATE(FResetHitArray);
DECLARE_DELEGATE(FEndSkill);
DECLARE_DELEGATE_OneParam(FToWeaponManager, UCDA_WeaponData*);

USTRUCT(Atomic, BlueprintType)
struct FSkillSlot
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName slotName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCDA_Skill * skill;

	bool operator==(FSkillSlot & data)
	{
		return this->slotName == data.slotName;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPP_POFOL_API UCSkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UCSkillManagerComponent();
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FToWeaponManager OnSpawnWeaponDelegate;
	FResetHitArray OnResetHitArrayDelegate;
	FEndSkill OnEndSkill;

public:

	UFUNCTION(BlueprintCallable)
	void LaunchSkill(FName key);


	UFUNCTION(BlueprintCallable)
	void EndSkill();

private:

	UFUNCTION()
	bool StartSkill(UCDA_Skill * skill);

		UFUNCTION()
		void StartSkill_Internal();

	UFUNCTION()
	void DoDamage(FHitResult& hitInform, EAttackType attackType);

////////////////////////////////////////////////////////////////////////////////
public://Get & Set

	const bool GetCanDoAttack() { return bCanDoAttack; }
	void SetCanDoAttack_true() { bCanDoAttack = true; }
	void SetCanDoAttack_false() { bCanDoAttack = false; }

	const float GetAttackYaw() { return attackYaw; }
	void SetAttackYaw(float yaw) { attackYaw = yaw; }

	const TSubclassOf<UCDamageType_Range> GetRangeDamageType() { return rangeDamageType; }
	void SetRangeDamageType(TSubclassOf<UCDamageType_Range> data) { rangeDamageType = data; }

	void SetDamageType(TSubclassOf<UCDamageType_Range> data) { rangeDamageType = data; }
	

////////////////////////////////////////////////////////////////////////////////
private://member variable

	UPROPERTY()
	ACCharacter * owner;

	UPROPERTY()
	UCGameInstance * gameInstance;

	//Character가 사용 할 스킬들 모음
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Skill Data", meta = (AllowPrivateAccess = true))
	TArray<FSkillSlot> skillSlots;

	//현재 사용중인 스킬
	UPROPERTY()
	UCDA_Skill * currentSkill;
	
	//현재 사용중인 Radial Damage type
	UPROPERTY()
	TSubclassOf<UCDamageType_Range> rangeDamageType;

	//콤보공격시 공격 인덱스
	int atkAnimIndex{ 0 };

	//공격이 가능한지
	bool bCanDoAttack{ true };

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool IsDebugMode{ false };

	//공격 처음 했을 때 컨트롤러의 Yaw값
	float attackYaw{ 0 };
};