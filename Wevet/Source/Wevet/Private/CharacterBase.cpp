// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "WeaponBase.h"
#include "Engine.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	DieSuccessCalled(false)
{
	PrimaryActorTick.bCanEverTick = true;
	this->BaseTurnRate   = 45.f;
	this->BaseLookUpRate = 45.f;
	this->MovementSpeed  = 300.f;
	this->IsCrouch = false;
	this->IsSprint = false;
	this->IsEquipWeapon = false;
	this->HeadSocketName = FName(TEXT("Head"));
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
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
	if (this->DieSuccessCalled)
	{
		return true;
	}
	return CharacterModel->GetCurrentHealth() <= 0;
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	if (this->IsDeath_Implementation()) 
	{
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
		if (this->SelectedWeapon->WeaponItemInfo.WeaponItemType == WeaponItemType)
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

