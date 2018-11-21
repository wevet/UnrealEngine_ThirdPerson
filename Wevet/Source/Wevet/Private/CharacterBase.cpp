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
	//
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
	if (this->IsDeath_Implementation())
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

void ACharacterBase::Die_Implementation()
{
	// twice called
	if (this->bDied)
	{
		return;
	}

	if (this->SelectedWeapon)
	{
		this->SelectedWeapon->OnFireRelease_Implementation();
		this->SelectedWeapon->SetCharacterOwner(nullptr);
	}
	Super::GetMesh()->SetAllBodiesSimulatePhysics(true);
	Super::GetMesh()->SetSimulatePhysics(true);
	Super::GetMesh()->WakeAllRigidBodies();
	Super::GetMesh()->bBlendPhysics = true;
	Super::GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (this->SelectedWeapon)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			const FQuat Rotation = Super::GetActorRotation().Quaternion();
			const FVector Forward = Super::GetActorLocation() + (Controller->GetControlRotation().Vector() * 200);
			FTransform Transform;
			Transform.SetLocation(Forward);
			Transform.SetRotation(Rotation);
			Transform.SetScale3D(FVector::OneVector);

			if (WeaponList.Find(this->SelectedWeapon) != INDEX_NONE)
			{
				WeaponList.Remove(this->SelectedWeapon);
			}
			FWeaponItemInfo WeaponItemInfo = SelectedWeapon->WeaponItemInfo;
			TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
			this->SelectedWeapon->Destroy();
			this->SelectedWeapon = nullptr;

			// spawn event
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = NULL;
			SpawnInfo.Instigator = NULL;
			AWeaponBase* const SpawningObject = World->SpawnActor<AWeaponBase>(WeaponClass, Transform.GetLocation(), Super::GetActorRotation(), SpawnInfo);
			SpawningObject->WeaponItemInfo.CopyTo(WeaponItemInfo);
			SpawningObject->OnVisible_Implementation();
		}
	}
	this->bDied = true;
}

void ACharacterBase::Equipment_Implementation()
{
	if (this->SelectedWeapon == nullptr) 
	{
		return;
	}
	this->SelectedWeapon->OnEquip(true);
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (this->SelectedWeapon == nullptr)
	{
		return;
	}
	this->SelectedWeapon->OnEquip(false);
}

// WeaponList Find Category
AWeaponBase* ACharacterBase::FindByWeapon(EWeaponItemType WeaponItemType)
{
	if (WeaponList.Num() <= 0 || this->SelectedWeapon == nullptr) 
	{
		return nullptr;
	}

	if (this->SelectedWeapon && this->SelectedWeapon->HasMatchTypes(WeaponItemType))
	{
		return this->SelectedWeapon;
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

// unequip weapon?
AWeaponBase* ACharacterBase::GetUnEquipWeapon()
{
	if (WeaponList.Num() <= 0)
	{
		return nullptr;
	}
	for (AWeaponBase* &Weapon : WeaponList)
	{
		if (Weapon->Equip == false)
		{
			return Weapon;
		}
	}
	return nullptr;
}

// pick already sameweapon category ?
const bool ACharacterBase::SameWeapon(AWeaponBase* Weapon)
{
	AWeaponBase* InWeapon = FindByWeapon(Weapon->WeaponItemInfo.WeaponItemType);
	if (InWeapon) 
	{
		bool bSame = InWeapon->WeaponItemInfo.WeaponItemType == Weapon->WeaponItemInfo.WeaponItemType;
		if (bSame)
		{
			UE_LOG(LogTemp, Warning, TEXT("SameWeapon : %s"), *(InWeapon->GetName()));
		}
		return bSame;
	}

	return false;
}

void ACharacterBase::EquipmentActionMontage()
{
	if (this->WeaponList.Num() <= 0)
	{
		return;
	}
	PlayAnimMontage(this->EquipMontage, 1.6f);
}

void ACharacterBase::FireActionMontage()
{
	if (this->WeaponList.Num() <= 0 || this->SelectedWeapon == nullptr)
	{
		return;
	}
	PlayAnimMontage(this->FireMontage);
}

void ACharacterBase::ReloadActionMontage()
{
	if (this->WeaponList.Num() <= 0 || this->SelectedWeapon == nullptr)
	{
		return;
	}
	PlayAnimMontage(this->ReloadMontage);
}
