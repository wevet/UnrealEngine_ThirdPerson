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
	this->MaxHealth      = 1.f;
	this->CurrentHealth  = 1.f;
	this->IsCrouch = false;
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
}

void ACharacterBase::SprintStarted()
{
	this->Sprint = true;
	MovementSpeed = FMath::Clamp<float>(MovementSpeed * 3.f, 300.f, this->DefaultMaxSpeed);
}

void ACharacterBase::SprintStopped()
{
	this->Sprint = false;
	MovementSpeed = FMath::Clamp<float>(MovementSpeed * 0.5f, 300.f, 300.f);
}

void ACharacterBase::UpdateSpeed()
{
	auto DeltaSeconds = GetWorld()->GetDeltaSeconds();
	auto Speed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, this->MovementSpeed, DeltaSeconds, 0.6);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
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

void ACharacterBase::OnReleaseItemExecuter_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Release"));
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor * Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Pick : %s"), *(Actor->GetName()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OnPickupItemExecuter_Implementation : %s"), *(Actor->GetName())));
}

void ACharacterBase::NotifyEquip_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("NotifyEquip : %s"), *(Super::GetName()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NotifyEquip_Implementation : %s"), *(Super::GetName())));
}

// @TODO
bool ACharacterBase::IsDeath_Implementation()
{
	bool IsDie = false;
	if (CharacterModel) 
	{
		IsDie = CharacterModel->GetCurrentHealth() <= 0;
	}
	return (this->CurrentHealth <= 0.0f) || IsDie;
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	// has already death?
	if (this->IsDeath_Implementation()) 
	{
		return;
	}
	if (BoneName == this->HeadSocketName) 
	{
		this->CurrentHealth = 0.f;
		if (CharacterModel) 
		{
			CharacterModel->SetCurrentHealthValue(0);
		}
		return;
	} 
	else
	{
		this->CurrentHealth = this->CurrentHealth - Damage;
		if (CharacterModel)
		{
			CharacterModel->SetCurrentHealthValue(CharacterModel->GetCurrentHealth() - (int)Damage);
		}
	}
}

void ACharacterBase::Die_Implementation()
{
	if (!this->DieSuccessCalled)
	{
		this->DieSuccessCalled = true;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Die_Implementation : %s"), *(Super::GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Die_Implementation : %s"), *(Super::GetName())));
}

void ACharacterBase::Equipment_Implementation()
{
	if (this->SelectedWeapon == nullptr) 
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Equipment : %s"), *(Super::GetName())));
	this->IsEquipWeapon = true;
	this->SelectedWeapon->OnEquip(this->IsEquipWeapon);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::FString("HandleUpdate Delta ") + FString::SanitizeFloat(Value));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: x: %s"), 10.0f));
}

void ACharacterBase::UnEquipment_Implementation()
{
	this->IsEquipWeapon = false;
	if (this->SelectedWeapon)
	{
		this->SelectedWeapon->OnEquip(this->IsEquipWeapon);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UnEquipment : %s"), *(Super::GetName())));
}


void ACharacterBase::UpdateCrouch(bool Crouch)
{
	this->IsCrouch = Crouch;
}

AWeaponBase* ACharacterBase::GetSelectedWeapon() const
{
	return this->SelectedWeapon;
}

TArray<AWeaponBase*> ACharacterBase::GetWeaponList() const
{
	return this->WeaponList;
}

AWeaponBase * ACharacterBase::GetCategoryByWeapon(EWeaponItemType WeaponItemType)
{
	if (this->WeaponList.Num() <= 0) {
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


void ACharacterBase::AttachWeapon()
{
	//
}

