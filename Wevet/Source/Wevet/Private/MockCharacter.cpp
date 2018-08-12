#include "MockCharacter.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

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
	GetCharacterMovement()->MaxWalkSpeed = Super::MovementSpeed;
}

void AMockCharacter::SprintStarted()
{
	this->Sprint = true;
	MovementSpeed = this->DefaultMaxSpeed;
}

void AMockCharacter::SprintStopped()
{
	this->Sprint = false;
	MovementSpeed = this->DefaultMaxSpeed *0.5f;
}

void AMockCharacter::UpdateSpeed()
{
	auto DeltaSeconds = GetWorld()->GetDeltaSeconds();
	auto Speed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, this->MovementSpeed, DeltaSeconds, 0.6);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

#pragma region InputAction
void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed,   this, &AMockCharacter::OnCrouch);
	PlayerInputComponent->BindAction("Jump",   IE_Pressed,   this, &AMockCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",   IE_Released,  this, &AMockCharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed,   this, &ACharacterBase::SprintStarted);
	PlayerInputComponent->BindAction("Sprint", IE_Released,  this, &ACharacterBase::SprintStopped);
	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMockCharacter::Equipment);
	PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed,  this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("DropItem", IE_Pressed, this, &AMockCharacter::ReleaseItem);

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
	this->Jump();
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
	if ((Controller != NULL) && (Value != 0.0f))
	{
		Super::UpdateSpeed();
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		Super::UpdateSpeed();
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMockCharacter::Equipment()
{
	if (this->WeaponList.Num() <= 0) 
	{
		return;
	}
	PlayAnimMontage(this->EquipMontage, 1.6f);
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
	Super::IsCrouch = !Super::IsCrouch;
	Super::UpdateCrouch(Super::IsCrouch);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Super::IsCrouch ? TEXT("Crouch!") : TEXT("UnCrouch!"));
}
#pragma endregion

// 
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
	UE_LOG(LogTemp, Warning, TEXT("WeaponCurrentIndex : %d"), this->WeaponCurrentIndex);
}

// current unequipment weapon
AWeaponBase* AMockCharacter::GetUnEquipedWeapon()
{
	if (WeaponList.Num() <= 0) 
	{
		return nullptr;
	}
	for (AWeaponBase* Weapon : WeaponList)
	{
		if (Weapon && Weapon->Equip == false) 
		{
			return Weapon;
		}
	}
	return nullptr;
}

// death
void AMockCharacter::Die_Implementation()
{
	GetCharacterMovement()->DisableMovement();	
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// stopfireevent
	if (Super::SelectedWeapon)
	{
		Super::SelectedWeapon->OnFireRelease_Implementation();
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

	if (IgnoreWeapon) 
	{
		IgnoreWeapon->OnVisible_Implementation();
	}
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
			UE_LOG(LogTemp, Warning, TEXT("UnRegistItem : %s"), *(Actor->GetName()));
			return;
		}
		TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = NULL;
		SpawnInfo.Instigator = NULL;
		const FTransform Transform = Super::GetMesh()->GetSocketTransform(WeaponItemInfo.UnEquipSocketName);
		AWeaponBase* PickingWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, Transform, SpawnInfo);

		if (this->WeaponList.Contains(PickingWeapon) == false)
		{
			this->WeaponList.Add(PickingWeapon);
		}
		PickingWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, WeaponItemInfo.UnEquipSocketName);
		PickingWeapon->OffVisible_Implementation();
		if (PickingWeapon->GetSphereComponent()) 
		{
			PickingWeapon->GetSphereComponent()->DestroyComponent();
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
		this->Die_Implementation();
	}
}

void AMockCharacter::NotifyEquip_Implementation()
{
	if (Super::SelectedWeapon) 
	{
		// detach weapon
		Super::SelectedWeapon->OnEquip(false);
		Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName);
		Super::SelectedWeapon = nullptr;
		Super::UnEquipment_Implementation();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		Super::bUseControllerRotationYaw = false;
	}  
	else
	{
		// attach weapon
		Super::SelectedWeapon = this->WeaponList[GetWeaponCurrentIndex()];
		Super::SelectedWeapon->OnEquip(true);
		Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, Super::SelectedWeapon->WeaponItemInfo.EquipSocketName);
		Super::Equipment_Implementation();
		GetCharacterMovement()->bOrientRotationToMovement = false;
		Super::bUseControllerRotationYaw = true;
	}
	//Super::NotifyEquip_Implementation();
}

AWeaponBase* AMockCharacter::ReleaseWeapon(const FTransform& Transform)
{
	auto IgnoreWeapon = GetUnEquipedWeapon();
	if (IgnoreWeapon)
	{
		// destory event
		FWeaponItemInfo WeaponItemInfo = IgnoreWeapon->WeaponItemInfo;
		TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
		WeaponList.Remove(IgnoreWeapon);
		IgnoreWeapon->Destroy();
		IgnoreWeapon = nullptr;

		// spawn event
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = NULL;
		SpawnInfo.Instigator = NULL;
		AWeaponBase* SpawningObject = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, Transform.GetLocation(), Super::GetActorRotation(), SpawnInfo);
		return SpawningObject;
	}
	return nullptr;
}

