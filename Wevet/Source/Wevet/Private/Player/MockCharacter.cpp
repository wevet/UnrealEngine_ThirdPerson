// Copyright 2018 wevet works All Rights Reserved.

#include "MockCharacter.h"
#include "CharacterPickupComponent.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RotationMatrix.h"
#include "CharacterAnimInstanceBase.h"
//#include "ShaderCompiler.h"

AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	WeaponCurrentIndex(INDEX_NONE)
{
	GetCapsuleComponent()->InitCapsuleSize(32.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 570.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoomComponent = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoomComponent"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->TargetArmLength = 240.0f;
	CameraBoomComponent->bUsePawnControlRotation = true;

	FollowCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCameraComponent"));
	FollowCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);
	FollowCameraComponent->bUsePawnControlRotation = false;
	FollowCameraComponent->SetRelativeLocation(FVector(0.f, 70.f, 30.f));
}

void AMockCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMockCharacter::BeginPlay()
{
	Super::BeginPlay();
	auto RefSkeleton = GetMesh()->SkeletalMesh->Skeleton->GetReferenceSkeleton();
	for (int i = 0; i < RefSkeleton.GetRawBoneNum(); ++i)
	{
		auto Info = RefSkeleton.GetRawRefBoneInfo()[i];
		//UE_LOG(LogWevetClient, Log, TEXT("BoneName : %s \n BoneIndex : %d"), *Info.Name.ToString(), Info.ParentIndex);
	}
}

void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// basic action
	PlayerInputComponent->BindAction("Crouch", IE_Pressed,   this, &ACharacterBase::OnCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed,   this, &ACharacterBase::OnSprint);
	PlayerInputComponent->BindAction("Jump",   IE_Pressed,   this, &AMockCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",   IE_Released,  this, &AMockCharacter::StopJumping);
	PlayerInputComponent->BindAxis("Turn",        this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",      this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate",    this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate",  this, &AMockCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMockCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",   this, &AMockCharacter::MoveRight);
#if !WITH_EDITOR
	PlayerInputComponent->BindTouch(IE_Pressed,   this, &AMockCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released,  this, &AMockCharacter::TouchStopped);
#endif
	// interaction
	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects",  IE_Pressed, this, &AMockCharacter::PickupObjects);

	// combat action
	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMockCharacter::EquipmentHandleEvent);
	PlayerInputComponent->BindAction("SwapWeapon",  IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("Fire", IE_Pressed,   this, &AMockCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released,  this, &AMockCharacter::FireReleassed);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::Reload);
}

void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if !WITH_EDITOR
void AMockCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Super::Jump();
}

void AMockCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	Super::StopJumping();
}
#endif

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
				IGrabExecuter::Execute_CanGrab(this, false);
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
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

		if (Super::bCanTurnLeft && bCanNotClimbJumpLeft)
		{
			Super::DisableInput(PC);
			IGrabExecuter::Execute_TurnConerLeftUpdate(this);
		}
		if (Super::bCanTurnRight && bCanNotClimbJumpRight)
		{
			Super::DisableInput(PC);
			IGrabExecuter::Execute_TurnConerRightUpdate(this);
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
	OnReleaseItemExecuter_Implementation();
}

void AMockCharacter::PickupObjects()
{
	OnPickupItemExecuter_Implementation(GetPickupComponent()->GetPickupActor());
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
	if (CurrentWeapon.IsValid())
	{
		BP_FirePressReceive();
	}
}

void AMockCharacter::FireReleassed()
{
	if (CurrentWeapon.IsValid())
	{
		BP_FireReleaseReceive();
	}
}

void AMockCharacter::Reload()
{
	if (CurrentWeapon.IsValid())
	{
		BP_ReloadReceive();
	}
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
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}

	if (WeaponCurrentIndex >= WeaponList.Num() - 1) 
	{
		WeaponCurrentIndex = 0;
	}
	else 
	{
		++WeaponCurrentIndex;
	}
}

void AMockCharacter::Die_Implementation()
{
	if (Super::bDied)
	{
		return;
	}

	if (APlayerController* PC = Wevet::ControllerExtension::GetPlayer(this, 0))
	{
		PC->UnPossess();
		Super::DisableInput(PC);
	}
	Super::Die_Implementation();
}

void AMockCharacter::OnReleaseItemExecuter_Implementation()
{
	ReleaseWeapon();
	Super::OnReleaseItemExecuter_Implementation();
}

void AMockCharacter::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (AWeaponBase* Weapon = Cast<AWeaponBase>(Actor))
	{
		if (Super::SameWeapon(Weapon))
		{
			//UE_LOG(LogWevetClient, Warning, TEXT("SameWeapon : %s"), *(Weapon->GetName()));
			return;
		}
		FWeaponItemInfo WeaponItemInfo = Weapon->WeaponItemInfo;
		if (WeaponItemInfo.WeaponItemType == EWeaponItemType::None)
		{
			return;
		}

		const FName SocketName(WeaponItemInfo.UnEquipSocketName);
		TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
		const FTransform Transform = Super::GetMesh()->GetSocketTransform(SocketName);
		AWeaponBase* const SpawningObject = World->SpawnActorDeferred<AWeaponBase>(
			WeaponClass,
			Transform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
		SpawningObject->FinishSpawning(Transform);
		SpawningObject->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		SpawningObject->Take(this);

		if (Super::WeaponList.Find(SpawningObject) == INDEX_NONE)
		{
			Super::WeaponList.Emplace(SpawningObject);
		}
		Weapon->Release(nullptr);
		Actor = nullptr;
	}

	if (AItemBase* Item = Cast<AItemBase>(Actor))
	{
		const EItemType ItemType = Item->GetItemType();
		switch (ItemType)
		{
		case EItemType::Weapon:
			{
				const FWeaponItemInfo ItemInfo = Item->WeaponItemInfo;
				if (auto Weapon = Super::FindByWeapon(ItemInfo.WeaponItemType))
				{
					Weapon->Recover(ItemInfo);
					Item->Release();
					Actor = nullptr;
				}
			}
			break;
		case EItemType::Health:
			// @TODO
			break;
		}
	}
	Super::OnPickupItemExecuter_Implementation(Actor);
}

void AMockCharacter::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor);
	if (ICombatExecuter::Execute_IsDeath(this))
	{
		Die_Implementation();
	}
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

	CurrentWeapon = MakeWeakObjectPtr<AWeaponBase>(WeaponList[WeaponCurrentIndex]);
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
	if (CurrentWeapon.IsValid() && CurrentWeapon.Get()->bEquip)
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
	IGrabExecuter::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
	IGrabExecuter::Execute_ClimbLedge(GetCharacterAnimInstance(), bClimbingLedge);
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
	//else if (FMath::IsNearlyZero(Value) && ClimbJumpUpMontage)
	//{
	//	PlayAnimMontage(ClimbJumpUpMontage);
	//}
	Super::ClimbJump_Implementation();
}

void AMockCharacter::ReportClimbJumpEnd_Implementation()
{
	if (auto Anim = GetCharacterAnimInstance())
	{
		Anim->Montage_Stop(0.f);
	}
	Super::ReportClimbJumpEnd_Implementation();
}

void AMockCharacter::TurnConerResult_Implementation()
{
	if (APlayerController* PC = ControllerExtension::GetPlayer(this, 0))
	{
		Super::EnableInput(PC);
	}
}

FVector AMockCharacter::BulletTraceRelativeLocation() const
{
	return GetFollowCameraComponent()->GetComponentLocation();
}

FVector AMockCharacter::BulletTraceForwardLocation() const
{
	return GetFollowCameraComponent()->GetForwardVector();
}

void AMockCharacter::EquipmentHandleEvent()
{
	if (CurrentWeapon.IsValid())
	{
		Super::UnEquipmentActionMontage();
	}
	else
	{
		Super::EquipmentActionMontage();
	}
}

void AMockCharacter::ReleaseWeapon()
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	const FVector ForwardOffset = Controller ? Controller->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward   = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD);
	const FTransform Transform  = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);
	
	if (AWeaponBase* Weapon = Super::GetUnEquipWeapon())
	{
		if (WeaponList.Find(Weapon) != INDEX_NONE)
		{
			WeaponList.Remove(Weapon);
		}
		Super::ReleaseWeaponToWorld(Transform, Weapon);
	}
}
