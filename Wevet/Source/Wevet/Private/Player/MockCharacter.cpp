// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockCharacter.h"
#include "Player/MockPlayerController.h"
#include "Component/CharacterPickupComponent.h"
#include "Component/CharacterInventoryComponent.h"
#include "AnimInstance/CharacterAnimInstanceBase.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RotationMatrix.h"
#include "WevetExtension.h"
#include "Item/AbstractItem.h"


AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	WeaponCurrentIndex(INDEX_NONE)
{
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.1f;

	JumpMaxHoldTime = 0.5f;

	BaseTurnRate = 150.f;
	BaseLookUpRate = 150.f;

	ViewPitchMin = -50.f;
	ViewPitchMax = 50.f;

	CameraBoomComponent = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoomComponent"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->TargetArmLength = 250.f;
	CameraBoomComponent->bUsePawnControlRotation = true;
	CameraBoomComponent->bEnableCameraLag = true;
	CameraBoomComponent->bDoCollisionTest = true;
	CameraBoomComponent->CameraLagSpeed = 2.0f;
	CameraBoomComponent->SocketOffset = FVector(0.f, 0.f, 45.f);

	TPSCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TPSCameraComponent"));
	TPSCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);
	TPSCameraComponent->bUsePawnControlRotation = false;
	TPSCameraComponent->SetRelativeLocation(FVector::ZeroVector);

	FPSCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FPSCameraComponent"));
	FPSCameraComponent->SetupAttachment(RootComponent);
	FPSCameraComponent->SetRelativeLocation(FVector(0.f, 10.f, 170.f));
	FPSCameraComponent->bUsePawnControlRotation = true;
	FPSCameraComponent->SetFieldOfView(100.f);
}

void AMockCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMockCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AMockPlayerController>(Wevet::ControllerExtension::GetPlayer(this));

	auto CameraMGR = Wevet::ControllerExtension::GetCameraManager(this);
	CameraMGR->ViewPitchMin = ViewPitchMin;
	CameraMGR->ViewPitchMax = ViewPitchMax;
}

void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region Input
void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects",  IE_Pressed, this, &AMockCharacter::PickupObjects);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMockCharacter::ToggleEquip);
	PlayerInputComponent->BindAction("Swap",  IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("Fire",  IE_Pressed,   this, &AMockCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire",  IE_Released,  this, &AMockCharacter::FireReleassed);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::Reload);

	// joystick Input
	PlayerInputComponent->BindAxis("LookRight", this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMockCharacter::LookUpAtRate);
}

void AMockCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMockCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMockCharacter::MoveForward(float Value)
{
	if (Super::bClimbJumping)
	{
		return;
	}
	if (Controller && Value != 0.0f)
	{
		// backward hanging reset
		if (Value <= 0.f)
		{
			if (Super::bHanging)
			{
				IGrabInstigator::Execute_CanGrab(this, false);
			}
		}
		else
		{
			if (Super::bHanging)
			{
				return;
			}
		}
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::MoveRight(float Value)
{
	if (Super::bClimbJumping)
	{
		return;
	}

	if (Super::bHanging)
	{
		const bool bCanNotClimbJumpLeft  = (!Super::bCanClimbJumpLeft && Value < 0.f);
		const bool bCanNotClimbJumpRight = (!Super::bCanClimbJumpRight && Value > 0.f);

		if (Super::bCanTurnLeft && bCanNotClimbJumpLeft)
		{
			Super::DisableInput(PlayerController);
			IGrabInstigator::Execute_TurnConerLeftUpdate(this);
		}
		if (Super::bCanTurnRight && bCanNotClimbJumpRight)
		{
			Super::DisableInput(PlayerController);
			IGrabInstigator::Execute_TurnConerRightUpdate(this);
		}
	}
	else
	{
		if (Controller && Value != 0.f)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Value);
		}
	}

}

void AMockCharacter::ReleaseObjects()
{
	Release_Implementation();
}

void AMockCharacter::PickupObjects()
{
	Pickup_Implementation(EItemType::None, GetPickupComponent()->GetPickupActor());
}

void AMockCharacter::Jump()
{
	if (Super::bClimbJumping)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using ClimbSystem.."));
		return;
	}
	else if (Super::bHanging)
	{
		const float Value = InputComponent->GetAxisValue(TEXT("MoveRight"));
		const bool bCanAnyLeftOrRightJump = (Super::bCanClimbJumpLeft || Super::bCanClimbJumpRight);
		//const bool bCanJumpUp = (Super::bCanClimbJumpUp && FMath::IsNearlyZero(Value));

		if (bCanAnyLeftOrRightJump)
		{
			ClimbJump_Implementation();
		}
		else if (!bCanAnyLeftOrRightJump && FMath::IsNearlyZero(Value))
		{
			Super::bClimbJumping = false;
			ClimbLedge_Implementation(true);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("Unknown state : %s"), *FString(__FUNCTION__));
		}
	}
	else
	{
		Super::Jump();
	}
}

void AMockCharacter::StopJumping()
{
	Super::StopJumping();
}

void AMockCharacter::FirePressed()
{
	Super::FirePressed();
}

void AMockCharacter::FireReleassed()
{
	Super::FireReleassed();
}

void AMockCharacter::Reload()
{
	Super::Reload();
}

void AMockCharacter::OnCrouch()
{
	if (Super::bHanging)
	{
		return;
	}
	Super::OnCrouch();

	if (Super::bCrouch)
	{
		Super::bSprint = false;
		MovementSpeed = DefaultMaxSpeed *0.5f;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
}

void AMockCharacter::UpdateWeapon()
{
	check(Super::InventoryComponent);	
	if (Super::InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}

	const TArray<AAbstractWeapon*> WeaponArray = Super::InventoryComponent->GetWeaponInventory();
	if (WeaponCurrentIndex >= WeaponArray.Num() - 1) 
	{
		WeaponCurrentIndex = 0;
	}
	else 
	{
		++WeaponCurrentIndex;
	}
}

void AMockCharacter::ToggleEquip()
{
	if (CurrentWeapon.IsValid())
	{
		Super::UnEquipmentActionMontage();
	}
	else
	{
		EquipmentActionMontage();
	}
}
#pragma endregion

void AMockCharacter::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}

	Super::DisableInput(PlayerController);
	Super::Die_Implementation();
}

void AMockCharacter::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor, bDied);
}

void AMockCharacter::Equipment_Implementation()
{
	if (Super::bHanging)
	{
		return;
	}

	if (WeaponCurrentIndex == INDEX_NONE)
	{
		UpdateWeapon();
	}

	const TArray<AAbstractWeapon*> WeaponItem = Super::InventoryComponent->GetWeaponInventoryOriginal();
	CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(WeaponItem[WeaponCurrentIndex]);
	if (!CurrentWeapon.IsValid())
	{
		return;
	}
	Super::Equipment_Implementation();
	const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.EquipSocketName);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	Super::bUseControllerRotationYaw = true;
}

void AMockCharacter::UnEquipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}
	Super::UnEquipment_Implementation();
	const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.UnEquipSocketName);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	Super::bUseControllerRotationYaw = false;
	CurrentWeapon.Reset();
}

void AMockCharacter::ClimbLedge_Implementation(bool InClimbLedge)
{
	if (CurrentWeapon.IsValid() && CurrentWeapon.Get()->WasEquip())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("UnEquip Weapon : [%s]"), *FString(__FUNCTION__));
		return;
	}
	if (bClimbingLedge == InClimbLedge)
	{
		return;
	}
	bClimbingLedge = InClimbLedge;
	if (!bHanging)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
	else
	{
		bHanging = false;
	}
	IGrabInstigator::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
	IGrabInstigator::Execute_ClimbLedge(GetCharacterAnimInstance(), bClimbingLedge);
	if (ClimbLedgeMontage)
	{
		PlayAnimMontage(ClimbLedgeMontage);
	}
}

void AMockCharacter::ClimbJump_Implementation()
{
	if (Super::bClimbJumping)
	{
		return;
	}
	check(bHanging);

	float Value = InputComponent->GetAxisValue(TEXT("MoveRight"));
	Value = FMath::Clamp<float>(Value, -1.f, 1.f);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	if (Value > 0.f && ClimbJumpRightMontage)
	{
		PlayAnimMontage(ClimbJumpRightMontage);
	}
	else if (Value < 0.f && ClimbJumpLeftMontage)
	{
		PlayAnimMontage(ClimbJumpLeftMontage);
	}
	Super::ClimbJump_Implementation();
}

void AMockCharacter::ReportClimbJumpEnd_Implementation()
{
	if (GetCharacterAnimInstance())
	{
		GetCharacterAnimInstance()->StopAllMontages(ZERO_VALUE);
	}
	Super::ReportClimbJumpEnd_Implementation();
}

void AMockCharacter::TurnConerResult_Implementation()
{
	if (PlayerController)
	{
		Super::EnableInput(PlayerController);
	}
}

// Always Pickup
const bool AMockCharacter::CanPickup_Implementation()
{
	return true;
}

void AMockCharacter::Release_Implementation()
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	const FVector ForwardOffset = Controller ? Controller->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (AAbstractWeapon * Weapon = Super::GetUnEquipWeapon())
	{
		Super::InventoryComponent->RemoveWeaponInventory(Weapon);
		Super::ReleaseWeaponToWorld(Transform, Weapon);
	}
}

void AMockCharacter::Pickup_Implementation(const EItemType InItemType, AActor* Actor)
{
	if (Actor == nullptr)
	{
		return;
	}

	if (AAbstractWeapon * Weapon = Cast<AAbstractWeapon>(Actor))
	{
		if (Super::SameWeapon(Weapon))
		{
			//UE_LOG(LogWevetClient, Warning, TEXT("SameWeapon : %s"), *(Weapon->GetName()));
			return;
		}
		// Create
		Super::CreateWeaponInstance(Weapon->GetClass());
		IInteractionInstigator::Execute_Release(Weapon, nullptr);
		Actor = nullptr;
	}

	if (AAbstractItem * Item = Cast<AAbstractItem>(Actor))
	{
		const EItemType ItemType = Item->GetItemType();
		switch (ItemType)
		{
		case EItemType::Weapon:
		{
			const FWeaponItemInfo ItemInfo = Item->WeaponItemInfo;
			if (auto Weapon = Super::FindByWeapon(ItemInfo.WeaponItemType))
			{
				IWeaponInstigator::Execute_DoReplenishment(Weapon, ItemInfo);
				IInteractionInstigator::Execute_Release(Item, nullptr);
				Actor = nullptr;
			}
		}
		break;
		case EItemType::Health:
			// @TODO
			break;
		}
	}
}

FVector AMockCharacter::BulletTraceRelativeLocation() const
{
	return GetTPSCameraComponent()->GetComponentLocation();
}

FVector AMockCharacter::BulletTraceForwardLocation() const
{
	return GetTPSCameraComponent()->GetForwardVector();
}

void AMockCharacter::EquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}

	if (WeaponCurrentIndex == INDEX_NONE)
	{
		UpdateWeapon();
	}

	// Fake WeaponPointer
	const TArray<AAbstractWeapon*> WeaponItem = Super::InventoryComponent->GetWeaponInventoryOriginal();
	auto WeaponPtr = WeaponItem[WeaponCurrentIndex];
	if (WeaponPtr)
	{
		const EWeaponItemType WeaponType = WeaponPtr->WeaponItemInfo.WeaponItemType;
		FWeaponActionInfo ActionInfo;
		SetActionInfo(WeaponType, ActionInfo);

		if (ActionInfo.EquipMontage)
		{
			PlayAnimMontage(ActionInfo.EquipMontage, MONTAGE_DELAY);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr AnimMontage : %s"), *FString(__FUNCTION__));
		}
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("Weapon nullptr : %s"), *FString(__FUNCTION__));
	}
}

