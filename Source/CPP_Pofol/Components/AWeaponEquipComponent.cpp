#include "AWeaponEquipComponent.h"
#include "GameFramework/Character.h"

//UE_LOG(LogTemp, Warning, TEXT("erode %s"), *data->GetStaticMesh()->GetName());


UAWeaponEquipComponent::UAWeaponEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	/*static ConstructorHelpers::FObjectFinder<UNiagaraSystem> UnSpawnEffect(TEXT("NiagaraSystem'/Game/Resources/Particles/Weapons/WeaponDistroy/NS_UnSpawnWeapon.NS_UnSpawnWeapon'"));

	unSpawnEffect = UnSpawnEffect.Object;*/

	//UAWeaponEquipComponent�迭 �ʱ�ȭ
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

	//Ÿ�Ӷ��� tick
	CurveTimeline.TickTimeline(DeltaTime);
}

void UAWeaponEquipComponent::ChangeWeapon(UCDA_WeaponData* weaponData)
{
	if (weaponData == nullptr) return;

	ACharacter * owner = Cast<ACharacter>(GetOwner());
	int index = 0;

	//���Ⱑ �ΰ��̻��� ��츦 ������ for�� ���
	for (FWeaponData& data : weaponData->weaponData)
	{
		//weapon static compoennt���� ���� ���� ���� �Ұ���
		if (weaponSlotMax <= index) break;

		//����ƽ�޽� ����
		WeaponStaticMeshes[index]->SetStaticMesh(data.weaponStaticmesh);
		
		//���͸��� ����
		int matIndex = 0;
		for (FStaticMaterial tempMaterial : data.weaponStaticmesh->StaticMaterials)
		{
			WeaponStaticMeshes[index]->SetMaterial(matIndex, tempMaterial.MaterialInterface);
			++matIndex;
		}

		//Owner skeletalMesh�� Attach
		WeaponStaticMeshes[index]->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, data.toBeEquipSocketName);

		++index;
	}

	//������� ���� ������ �޽� �Ⱥ��̰� nulló��, ���� ������� �ʾƵ� �ȴ�?
	for ( ; index < weaponSlotMax; index++)
	{
		WeaponStaticMeshes[index]->SetStaticMesh(nullptr);
	}

	//�ڿ������� ������ ���� ���͸��� �� ���� �� Ÿ�Ӷ��� ����
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

			//Ÿ�Ӷ����� �������̸� 0���� �����ص� �ٽ� Ÿ�Ӷ��ΰ����� �����Ǳ⶧���� �ߴ� �������
			if (CurveTimeline.IsPlaying() == true)
			{
				CurveTimeline.Stop();
			}
			data->SetScalarParameterValueOnMaterials(FName("erode_alpha"), 0.0f);
		}
	}
	return;
}