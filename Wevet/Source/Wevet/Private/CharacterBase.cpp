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
	this->IsSprint = !this->IsSprint;

	// now crouching slow speed
	if (this->IsCrouch)
	{
		this->IsSprint = false;
	}
	MovementSpeed = this->IsSprint ? this->DefaultMaxSpeed : this->DefaultMaxSpeed *0.5f;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	this->IsCrouch = !this->IsCrouch;
}

void ACharacterBase::OnReleaseItemExecuter_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Release"));
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor * Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Pick : %s"), *(Actor->GetName()));
}

void ACharacterBase::NotifyEquip_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("NotifyEquip : %s"), *(Super::GetName()));
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
	if (this->DieSuccessCalled)
	{
		return;
	}

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
			AWeaponBase* SpawningObject = World->SpawnActor<AWeaponBase>(WeaponClass, Transform.GetLocation(), Super::GetActorRotation(), SpawnInfo);
			SpawningObject->WeaponItemInfo.CopyTo(WeaponItemInfo);
			SpawningObject->OnVisible_Implementation();
		}
	}

	DieSuccessCalled = true;

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

AWeaponBase* ACharacterBase::FindByWeapon(EWeaponItemType WeaponItemType)
{
	if (WeaponList.Num() <= 0 || SelectedWeapon == nullptr) 
	{
		return nullptr;
	}

	for (AWeaponBase*& Weapon : this->WeaponList)
	{
		if (SelectedWeapon)
		{
			if (SelectedWeapon->HasMatchTypes(WeaponItemType))
			{
				return SelectedWeapon;
			}
		}
		else 
		{
			if (Weapon && Weapon->HasMatchTypes(WeaponItemType))
			{
				return Weapon;
			}
		}

	}
	return nullptr;
}

void ACharacterBase::EquipmentMontage()
{
	if (this->WeaponList.Num() <= 0)
	{
		return;
	}
	PlayAnimMontage(EquipMontage, 1.6f);
}

