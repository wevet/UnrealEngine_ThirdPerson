// Copyright 2018 wevet works All Rights Reserved.

#include "MockCharacter.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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
}

#pragma region InputAction
void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed,   this, &ACharacterBase::OnCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed,   this, &ACharacterBase::OnSprint);
	PlayerInputComponent->BindAction("Jump",   IE_Pressed,   this, &AMockCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",   IE_Released,  this, &AMockCharacter::StopJumping);
	// @NOTE 
	// CharacterBase class
	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &ACharacterBase::EquipmentActionMontage);
	PlayerInputComponent->BindAction("SwapWeapon",  IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("DropItem",    IE_Pressed, this, &AMockCharacter::ReleaseItem);
	PlayerInputComponent->BindAction("Fire", IE_Pressed,   this, &AMockCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released,  this, &AMockCharacter::FireReleassed);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::Reload);

	PlayerInputComponent->BindAxis("Turn",        this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",      this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate",    this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate",  this, &AMockCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMockCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",   this, &AMockCharacter::MoveRight);
	PlayerInputComponent->BindTouch(IE_Pressed,   this, &AMockCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released,  this, &AMockCharacter::TouchStopped);
}

void AMockCharacter::ReleaseItem()
{
	OnReleaseItemExecuter_Implementation();
}

void AMockCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Super::Jump();
}

void AMockCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	Super::StopJumping();
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
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::FirePressed()
{
	if (Super::SelectedWeapon)
	{
		BP_FirePressReceive();
	}
}

void AMockCharacter::FireReleassed()
{
	if (Super::SelectedWeapon)
	{
		BP_FireReleaseReceive();
	}
}

void AMockCharacter::Reload()
{
	if (Super::SelectedWeapon)
	{
		BP_ReloadReceive();
	}
}

void AMockCharacter::Jump()
{
	if (Super::IsCrouch)
	{
		return;
	}
	Super::Jump();
}

void AMockCharacter::StopJumping()
{
	Super::StopJumping();
}

void AMockCharacter::OnCrouch()
{
	Super::OnCrouch();

	if (Super::IsCrouch)
	{
		Super::IsSprint = false;
		MovementSpeed = this->DefaultMaxSpeed *0.5f;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
}
#pragma endregion
 
void AMockCharacter::UpdateWeapon()
{
	if (this->WeaponList.Num() <= 0)
	{
		return;
	}

	if (this->WeaponCurrentIndex >= this->WeaponList.Num() - 1) 
	{
		this->WeaponCurrentIndex = 0;
	}
	else 
	{
		++this->WeaponCurrentIndex;
	}
}

// death
void AMockCharacter::Die_Implementation()
{
	if (Super::DieSuccessCalled)
	{
		return;
	}

	auto Controller = UGameplayStatics::GetPlayerController(this, 0);
	if (Controller)
	{
		Super::DisableInput(Controller);
	}
	Super::Die_Implementation();
}

// release weapon base
void AMockCharacter::OnReleaseItemExecuter_Implementation()
{
	const FRotator Rotation = Super::GetActorRotation();	
	const FVector Forward   = Super::GetActorLocation() + (Controller->GetControlRotation().Vector() * 200);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	AWeaponBase* IgnoreWeapon = ReleaseWeapon(Transform);
	Super::OnReleaseItemExecuter_Implementation();
}

// pick up
void AMockCharacter::OnPickupItemExecuter_Implementation(AActor * Actor)
{
	AWeaponBase* Weapon = Cast<AWeaponBase>(Actor);
	if (Weapon) 
	{
		FWeaponItemInfo WeaponItemInfo = Weapon->WeaponItemInfo;
		if (WeaponItemInfo.WeaponItemType == EWeaponItemType::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("UnKnownItemType : %s"), *(Weapon->GetName()));
			return;
		}
		TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = NULL;
		SpawnInfo.Instigator = NULL;
		const FTransform Transform = Super::GetMesh()->GetSocketTransform(WeaponItemInfo.UnEquipSocketName);
		AWeaponBase* PickingWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, Transform, SpawnInfo);

		PickingWeapon->AttachToComponent(
			Super::GetMesh(), 
			{ EAttachmentRule::SnapToTarget, true }, 
			WeaponItemInfo.UnEquipSocketName);
		PickingWeapon->OffVisible_Implementation();
		PickingWeapon->OnEquip(false);

		if (PickingWeapon->GetSphereComponent()) 
		{
			PickingWeapon->GetSphereComponent()->DestroyComponent();
		}
		if (Super::WeaponList.Contains(PickingWeapon) == false)
		{
			Super::WeaponList.Add(PickingWeapon);
		}
		Weapon->Destroy();
		Weapon = nullptr;
	} 
	Super::OnPickupItemExecuter_Implementation(Actor);
}

void AMockCharacter::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor);
	if (Super::IsDeath_Implementation())
	{
		Die_Implementation();
	}
}

void AMockCharacter::NotifyEquip_Implementation()
{

	if (Super::SelectedWeapon) 
	{
		// detach weapon
		Super::SelectedWeapon->AttachToComponent(
			Super::GetMesh(), 
			{ EAttachmentRule::SnapToTarget, true }, 
			Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName);
		
		Super::UnEquipment_Implementation();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		Super::bUseControllerRotationYaw = false;
		Super::SelectedWeapon = nullptr;
	}  
	else
	{
		// attach weapon
		Super::SelectedWeapon = this->WeaponList[this->WeaponCurrentIndex];
		Super::SelectedWeapon->AttachToComponent(
			Super::GetMesh(), 
			{ EAttachmentRule::SnapToTarget, true }, 
			Super::SelectedWeapon->WeaponItemInfo.EquipSocketName);

		Super::Equipment_Implementation();
		GetCharacterMovement()->bOrientRotationToMovement = false;
		Super::bUseControllerRotationYaw = true;
	}
	Super::NotifyEquip_Implementation();
}

FVector AMockCharacter::BulletTraceRelativeLocation() const
{
	return GetFollowCameraComponent()->GetComponentLocation();
}

FVector AMockCharacter::BulletTraceForwardLocation() const
{
	return GetFollowCameraComponent()->GetForwardVector();
}

AWeaponBase* AMockCharacter::ReleaseWeapon(const FTransform& Transform)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return nullptr;
	}

	AWeaponBase* UnEquipWeapon = Super::GetUnEquipWeapon();
	if (UnEquipWeapon)
	{
		if (WeaponList.Contains(UnEquipWeapon)) 
		{
			WeaponList.Remove(UnEquipWeapon);
		}

		FWeaponItemInfo WeaponItemInfo = UnEquipWeapon->WeaponItemInfo;
		TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
		UnEquipWeapon->Destroy();
		UnEquipWeapon = nullptr;

		// spawn event
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = NULL;
		SpawnInfo.Instigator = NULL;
		AWeaponBase* const SpawningObject = World->SpawnActor<AWeaponBase>(WeaponClass, Transform.GetLocation(), Super::GetActorRotation(), SpawnInfo);
		SpawningObject->WeaponItemInfo.CopyTo(WeaponItemInfo);
		SpawningObject->OnVisible_Implementation();
		return SpawningObject;
	}
	return nullptr;
}

