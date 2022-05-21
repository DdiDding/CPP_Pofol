#include "AWeaponEquipComponent.h"
#include "GameFramework/Character.h"

//UE_LOG(LogTemp, Warning, TEXT("erode %s"), *data->GetStaticMesh()->GetName());


UAWeaponEquipComponent::UAWeaponEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	/*static ConstructorHelpers::FObjectFinder<UNiagaraSystem> UnSpawnEffect(TEXT("NiagaraSystem'/Game/Resources/Particles/Weapons/WeaponDistroy/NS_UnSpawnWeapon.NS_UnSpawnWeapon'"));

	unSpawnEffect = UnSpawnEffect.Object;*/

	//UAWeaponEquipComponent배열 초기화
	{
		weaponSlotMax = 4;
		WeaponStaticMeshes.Init(nullptr, weaponSlotMax);
		
		int num = 0;
		for (UStaticMeshComponent*& data : WeaponStaticMeshes)
		{
			FString subObjectName = "WeaponSlot" + FString::FromInt(num);
			data = CreateDefaultSubobject<UStaticMeshComponent>(FName(*subObjectName));
			num++;
		}
	}
}

void UAWeaponEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("SpawnWeaponMaterial"));
		CurveTimeline.AddInterpFloat(CurveFloat, TimelineProgress);

		/*FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("SetWeaponMaterial"));
		CurveTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);*/
	}
}


void UAWeaponEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//타임라인 tick
	CurveTimeline.TickTimeline(DeltaTime);
}

void UAWeaponEquipComponent::ChangeWeapon(UCDA_WeaponData* weaponData)
{
	if (weaponData == nullptr) return;

	ACharacter * owner = Cast<ACharacter>(GetOwner());
	int index = 0;

	//무기가 두개이상일 경우를 생각해 for문 사용
	for (FWeaponData& data : weaponData->weaponData)
	{
		//weapon static compoennt개수 보다 많게 적용 불가능
		if (weaponSlotMax <= index) break;

		//스태틱메시 변경
		WeaponStaticMeshes[index]->SetStaticMesh(data.weaponStaticmesh);
		
		//머터리얼 변경
		int matIndex = 0;
		for (FStaticMaterial tempMaterial : data.weaponStaticmesh->StaticMaterials)
		{
			WeaponStaticMeshes[index]->SetMaterial(matIndex, tempMaterial.MaterialInterface);
			++matIndex;
		}

		//Owner skeletalMesh에 Attach
		WeaponStaticMeshes[index]->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, data.toBeEquipSocketName);

		++index;
	}

	//사용하지 않은 슬롯은 메시 안보이게 null처리, 굳이 떼어놓지 않아도 된다?
	for ( ; index < weaponSlotMax; index++)
	{
		WeaponStaticMeshes[index]->SetStaticMesh(nullptr);
	}

	//자연스러운 생성을 위해 머터리얼 값 조절 할 타임라인 실행
	CurveTimeline.PlayFromStart();

	return;
}

void UAWeaponEquipComponent::SpawnWeaponMaterial(float value)
{

	for (UStaticMeshComponent*& data : WeaponStaticMeshes)
	{
		if (data->GetStaticMesh() != nullptr)
		{
			data->SetScalarParameterValueOnMaterials(FName("erode_alpha"), value);
		}
	}
	
	return;
}

void UAWeaponEquipComponent::UnSpawnWeapon()
{
	for (UStaticMeshComponent*& data : WeaponStaticMeshes)
	{
		if (data->GetStaticMesh() != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(unSpawnEffect, data, TEXT("None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true, true, ENCPoolMethod::AutoRelease, true);

			//타임라인이 실행중이면 0으로 설정해도 다시 타임라인값으로 설정되기때문에 중단 해줘야함
			if (CurveTimeline.IsPlaying() == true)
			{
				CurveTimeline.Stop();
			}
			data->SetScalarParameterValueOnMaterials(FName("erode_alpha"), 0.0f);
		}
	}
	return;
}