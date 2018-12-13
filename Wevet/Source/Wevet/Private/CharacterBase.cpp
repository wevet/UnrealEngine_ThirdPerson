// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterBase.h"
#include "WeaponBase.h"
#include "CharacterModel.h"
#include "CharacterPickupComponent.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogWevetClient);

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	MovementSpeed(300.f),
	HeadSocketName(FName(TEXT("Head")))
{
	PrimaryActorTick.bCanEverTick = true;
	bCrouch = false;
	bSprint = false;
	bDied   = false;
	PawnNoiseEmitterComponent = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("PawnNoiseEmitterComponent"));
	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->SetupAttachment(GetMesh());
	PickupComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterPickupComponent>(this, TEXT("PickupComponent"));
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::BeginDestroy()
{
	if (CharacterModel)
	{
		CharacterModel->ConditionalBeginDestroy();
	}
	Super::BeginDestroy();
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterBase::Jump()
{
	Super::Jump();
}

void ACharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ACharacterBase::OnSprint()
{
	bSprint = !bSprint;

	// now crouching slow speed
	if (bCrouch)
	{
		bSprint = false;
	}
	MovementSpeed = bSprint ? DefaultMaxSpeed : DefaultMaxSpeed *0.5f;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	bCrouch = !bCrouch;
}

void ACharacterBase::OnReleaseItemExecuter_Implementation() 
{
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Picking : %s"), *(Actor->GetName()));
	}
}

void ACharacterBase::NotifyEquip_Implementation() 
{
}

void ACharacterBase::ReportNoise_Implementation(USoundBase* Sound, float Volume)
{
	UWorld* const World = GetWorld();
	float InVolume = (GetCharacterMovement()->MaxWalkSpeed / MovementSpeed);
	UE_LOG(LogWevetClient, Log, TEXT("Vol : %f"), InVolume);

	if (Sound && World)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, GetActorLocation());
		MakeNoise(Volume, this, GetActorLocation());
	}
}

bool ACharacterBase::IsDeath_Implementation()
{
	if (bDied || CharacterModel == nullptr)
	{
		return true;
	}
	return CharacterModel->GetCurrentHealth() <= 0;
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	if (IsDeath_Implementation())
	{
		return;
	}

	// Character & Target Same Class
	if (ICombatExecuter* Combat = Cast<ICombatExecuter>(Actor))
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Victim : %s"), *Actor->GetName());
		UE_LOG(LogWevetClient, Warning, TEXT("Receive Name : %s"), *GetName());
		//if (GetOwnerClass_Implementation() == Combat->GetOwnerClass_Implementation())
		//{
		//	return;
		//}
	}

	if (BoneName == HeadSocketName) 
	{
		CharacterModel->SetCurrentHealthValue(0);
	} 
	else
	{
		int32 TakeDamage = (int32)(FMath::Abs(Damage));
		int32 CurrentHealth = CharacterModel->GetCurrentHealth();
		CharacterModel->SetCurrentHealthValue(CurrentHealth - TakeDamage);
	}

}

// All deploy weapon
void ACharacterBase::Die_Implementation()
{
	if (bDied)
	{
		return;
	}

	SelectedWeapon = nullptr;
	Super::GetMesh()->SetAllBodiesSimulatePhysics(true);
	Super::GetMesh()->SetSimulatePhysics(true);
	Super::GetMesh()->WakeAllRigidBodies();
	Super::GetMesh()->bBlendPhysics = true;
	Super::GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}

	const FQuat Rotation = Super::GetActorRotation().Quaternion();
	const FVector Forward = Super::GetActorLocation() + (Controller->GetControlRotation().Vector() * 200);
	FTransform Transform;
	Transform.SetLocation(Forward);
	Transform.SetRotation(Rotation);
	Transform.SetScale3D(FVector::OneVector);

	if (UWorld* const World = GetWorld())
	{
		for (AWeaponBase*& Weapon : WeaponList)
		{
			if (!Weapon)
			{
				checkSlow(0);
				continue;
			}
			Weapon->OnFireRelease_Implementation();
			Weapon->SetCharacterOwner(nullptr);
			Weapon->SetEquip(false);

			FWeaponItemInfo& WeaponItemInfo = Weapon->WeaponItemInfo;
			TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
			Weapon->Destroy();
			Weapon = nullptr;

			// spawn event
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = NULL;
			SpawnInfo.Instigator = NULL;
			AWeaponBase* const SpawningObject = World->SpawnActor<AWeaponBase>(WeaponClass, Transform.GetLocation(), GetActorRotation(), SpawnInfo);
			SpawningObject->CopyTo(WeaponItemInfo);
			SpawningObject->OnVisible_Implementation();
		}
		WeaponList.Empty();
	}
	bDied = true;
}

void ACharacterBase::Equipment_Implementation()
{
	if (SelectedWeapon) 
	{
		SelectedWeapon->SetEquip(true);
	}
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (SelectedWeapon)
	{
		SelectedWeapon->SetEquip(false);
	}
}

const bool ACharacterBase::HasEquipWeapon()
{
	if (SelectedWeapon == nullptr)
	{
		return false;
	}
	return SelectedWeapon->bEquip;
}

float ACharacterBase::GetHealthToWidget() const
{
	return CharacterModel->GetHealthToWidget();
}

// WeaponList Find Category
AWeaponBase* ACharacterBase::FindByWeapon(EWeaponItemType WeaponItemType)
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return nullptr;
	}

	for (AWeaponBase*& Weapon : WeaponList)
	{
		if (Weapon && Weapon->HasMatchTypes(WeaponItemType))
		{
			return Weapon;
		}
	}
	return nullptr;
}

// unequip weapon first index
AWeaponBase* ACharacterBase::GetUnEquipWeapon()
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return nullptr;
	}
	for (AWeaponBase* &Weapon : WeaponList)
	{
		if (!Weapon->bEquip)
		{
			return Weapon;
		}
	}
	return nullptr;
}

// out unequip weaponlist
void ACharacterBase::OutUnEquipWeaponList(TArray<AWeaponBase*>& OutWeaponList)
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}
	for (AWeaponBase* &Weapon : WeaponList)
	{
		if (!Weapon)
		{
			checkSlow(0);
			continue;
		}
		if (!Weapon->bEquip)
		{
			OutWeaponList.Emplace(Weapon);
		}
	}
}

// pick already sameweapon category ?
const bool ACharacterBase::SameWeapon(AWeaponBase* Weapon)
{
	if (AWeaponBase* InWeapon = FindByWeapon(Weapon->WeaponItemInfo.WeaponItemType))
	{
		return InWeapon->WeaponItemInfo.WeaponItemType == Weapon->WeaponItemInfo.WeaponItemType;
	}
	return false;
}

void ACharacterBase::PickupObjects()
{
}

void ACharacterBase::ReleaseObjects()
{
}

/*********************************
CharacterBaseClass
AnimationMontage Event
*********************************/
void ACharacterBase::EquipmentActionMontage()
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}
	PlayAnimMontage(EquipMontage, 1.6f);
}

void ACharacterBase::FireActionMontage()
{
	if (ArrayExtension::NullOrEmpty(WeaponList) || SelectedWeapon == nullptr)
	{
		return;
	}
	PlayAnimMontage(FireMontage);
}

void ACharacterBase::ReloadActionMontage()
{
	if (ArrayExtension::NullOrEmpty(WeaponList) || SelectedWeapon == nullptr)
	{
		return;
	}
	PlayAnimMontage(ReloadMontage);
}
