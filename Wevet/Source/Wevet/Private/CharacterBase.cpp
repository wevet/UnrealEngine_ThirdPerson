// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterBase.h"
#include "WeaponBase.h"
#include "Engine.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	DieSuccessCalled(false),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	MovementSpeed(300.f),
	HeadSocketName(FName(TEXT("Head")))
{
	PrimaryActorTick.bCanEverTick = true;
	this->IsCrouch = false;
	this->IsSprint = false;
	this->IsEquipWeapon = false;
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

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	this->CharacterModel  = NewObject<UCharacterModel>();
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
	this->IsSprint = !this->IsSprint;

	// now crouching slow speed
	if (this->IsCrouch)
	{
		this->IsSprint = false;
	}
	
	if (this->IsSprint)
	{
		MovementSpeed = this->DefaultMaxSpeed;
	}
	else
	{
		MovementSpeed = this->DefaultMaxSpeed *0.5f;
	}
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	this->IsCrouch = !this->IsCrouch;
}

void ACharacterBase::OnReleaseItemExecuter_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Release"));
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor * Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Pick : %s"), *(Actor->GetName()));
}

void ACharacterBase::NotifyEquip_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("NotifyEquip : %s"), *(Super::GetName()));
}

bool ACharacterBase::IsDeath_Implementation()
{
	if (this->DieSuccessCalled || this->CharacterModel == nullptr)
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

	if (this->CharacterModel == nullptr)
	{
		if (!this->DieSuccessCalled) 
		{
			this->DieSuccessCalled = true;
		}
		return;
	}

	if (BoneName == this->HeadSocketName) 
	{
		CharacterModel->SetCurrentHealthValue(0);
		return;
	} 
	else
	{
		int TakeDamage = (int)(FMath::Abs(Damage));
		CharacterModel->SetCurrentHealthValue(CharacterModel->GetCurrentHealth() - TakeDamage);
	}
}

void ACharacterBase::Die_Implementation()
{
	if (!this->DieSuccessCalled)
	{
		if (this->SelectedWeapon)
		{
			this->SelectedWeapon->OnFireRelease_Implementation();
		}
		Super::GetMesh()->SetAllBodiesSimulatePhysics(true);
		Super::GetMesh()->SetSimulatePhysics(true);
		Super::GetMesh()->WakeAllRigidBodies();
		Super::GetMesh()->bBlendPhysics = true;
		Super::GetCharacterMovement()->DisableMovement();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->CharacterModel->ConditionalBeginDestroy();
		this->DieSuccessCalled = true;
	}
}

void ACharacterBase::Equipment_Implementation()
{
	if (this->SelectedWeapon == nullptr) 
	{
		return;
	}
	this->IsEquipWeapon = true;
	this->SelectedWeapon->OnEquip(this->IsEquipWeapon);
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (this->SelectedWeapon == nullptr)
	{
		return;
	}
	this->IsEquipWeapon = false;
	this->SelectedWeapon->OnEquip(this->IsEquipWeapon);
}

AWeaponBase* ACharacterBase::GetCategoryByWeapon(EWeaponItemType WeaponItemType)
{
	if (this->WeaponList.Num() <= 0) 
	{
		return nullptr;
	}

	if (this->SelectedWeapon)
	{
		if (this->SelectedWeapon->HasMatchTypes(WeaponItemType))
		{
			return this->SelectedWeapon;
		}
	}
	else 
	{
		for (AWeaponBase* Weapon : this->WeaponList)
		{
			if (Weapon->WeaponItemInfo.WeaponItemType == WeaponItemType)
			{
				return Weapon;
			}
		}
	}
	return nullptr;
}

