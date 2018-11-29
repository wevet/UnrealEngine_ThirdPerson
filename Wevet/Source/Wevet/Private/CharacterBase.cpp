// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterBase.h"
#include "WeaponBase.h"
#include "Engine.h"

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
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::BeginDestroy()
{
	if (this->CharacterModel)
	{
		this->CharacterModel->ConditionalBeginDestroy();
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
	this->DefaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = this->MovementSpeed;
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
	this->bSprint = !this->bSprint;

	// now crouching slow speed
	if (this->bCrouch)
	{
		this->bSprint = false;
	}
	MovementSpeed = this->bSprint ? this->DefaultMaxSpeed : this->DefaultMaxSpeed *0.5f;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	this->bCrouch = !this->bCrouch;
}

void ACharacterBase::OnReleaseItemExecuter_Implementation()
{
	//
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogTemp, Log, TEXT("Picking : %s"), *(Actor->GetName()));
	}
}

void ACharacterBase::NotifyEquip_Implementation()
{
	//
}

bool ACharacterBase::IsDeath_Implementation()
{
	if (this->bDied || this->CharacterModel == nullptr)
	{
		return true;
	}
	return this->CharacterModel->GetCurrentHealth() <= 0;
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	if (IsDeath_Implementation())
	{
		return;
	}

	if (BoneName == this->HeadSocketName) 
	{
		this->CharacterModel->SetCurrentHealthValue(0);
		return;
	} 
	else
	{
		if (this->CharacterModel)
		{
			int32 TakeDamage = (int32)(FMath::Abs(Damage));
			int32 CurrentHealth = this->CharacterModel->GetCurrentHealth();
			this->CharacterModel->SetCurrentHealthValue(CurrentHealth - TakeDamage);
		}
		else
		{
			Die_Implementation();
		}
	}
}

// All deploy weapon
void ACharacterBase::Die_Implementation()
{
	// twice called
	if (this->bDied)
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

	const bool bHasWeapon = (WeaponList.Num() <= 0);
	if (bHasWeapon)
	{
		return;
	}

	const FQuat Rotation = Super::GetActorRotation().Quaternion();
	const FVector Forward = Super::GetActorLocation() + (Controller->GetControlRotation().Vector() * 200);
	FTransform Transform;
	Transform.SetLocation(Forward);
	Transform.SetRotation(Rotation);
	Transform.SetScale3D(FVector::OneVector);

	UWorld* World = GetWorld();
	if (World)
	{

		for (AWeaponBase*& Weapon : WeaponList)
		{
			if (!Weapon)
			{
				check(0);
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
	this->bDied = true;
}

void ACharacterBase::Equipment_Implementation()
{
	if (SelectedWeapon == nullptr) 
	{
		return;
	}
	SelectedWeapon->SetEquip(true);
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (SelectedWeapon == nullptr)
	{
		return;
	}
	SelectedWeapon->SetEquip(false);
}

// WeaponList Find Category
AWeaponBase* ACharacterBase::FindByWeapon(EWeaponItemType WeaponItemType)
{
	if (WeaponList.Num() <= 0)
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
	if (WeaponList.Num() <= 0)
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
	if (WeaponList.Num() <= 0) 
	{
		return;
	}
	for (AWeaponBase* &Weapon : WeaponList)
	{
		if (Weapon == nullptr)
		{
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
	AWeaponBase* InWeapon = FindByWeapon(Weapon->WeaponItemInfo.WeaponItemType);
	if (InWeapon) 
	{
		return InWeapon->WeaponItemInfo.WeaponItemType == Weapon->WeaponItemInfo.WeaponItemType;
	}
	return false;
}

#pragma region Montage
void ACharacterBase::EquipmentActionMontage()
{
	if (WeaponList.Num() <= 0)
	{
		return;
	}
	PlayAnimMontage(EquipMontage, 1.6f);
}

void ACharacterBase::FireActionMontage()
{
	if (WeaponList.Num() <= 0 || SelectedWeapon == nullptr)
	{
		return;
	}
	PlayAnimMontage(FireMontage);
}

void ACharacterBase::ReloadActionMontage()
{
	if (WeaponList.Num() <= 0 || SelectedWeapon == nullptr)
	{
		return;
	}
	PlayAnimMontage(ReloadMontage);
}
#pragma endregion
