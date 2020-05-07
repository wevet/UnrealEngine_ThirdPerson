// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockCharacter.h"
#include "Player/MockPlayerController.h"
#include "AnimInstance/PlayerAnimInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RotationMatrix.h"

#include "Wevet.h"
#include "WevetExtension.h"
#include "Net/UnrealNetwork.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.0f);

	bReplicates = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// override
	GetMesh()->SetCollisionProfileName(FName(TEXT("CharacterMesh")));
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.1f;

	bEnableRagdoll = false;
	JumpMaxHoldTime = 0.5f;
	BaseTurnRate = 150.f;
	BaseLookUpRate = 150.f;

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

	WeaponCurrentIndex = INDEX_NONE;
	RecoverHealthValue = 100;
	RecoverTimer = 2.0f;
	Tags.Add(FName(TEXT("DamageInstigator")));

	// ALS
	bWasMovementInput = false;
	bWasMoving = false;
	bAiming = false;
	bRightShoulder = true;
	bRagdollOnGround = false;
	RagdollPoseSnapshot = FName(TEXT("RagdollPose"));
	// ALS Override
	GetCharacterMovement()->BrakingFrictionFactor = ZERO_VALUE;
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;
}

void AMockCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FRotator Rotation = GetActorRotation();
	LastVelocityRotation = Rotation;
	LookingRotation = Rotation;
	LastMovementInputRotation = Rotation;
	TargetRotation = Rotation;
	CharacterRotation = Rotation;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	FPSCameraComponent->AttachToComponent(GetMesh(), Rules, FPS_SOCKET);
	FPSCameraComponent->SetRelativeRotation(FRotator(0.f, 90.0f, -90.f));
}

void AMockCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MeshArray.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void AMockCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AMockPlayerController>(Wevet::ControllerExtension::GetPlayer(this));
	PlayerAnimInstance = Cast<UPlayerAnimInstance>(GetAnimInstance());
	MeshArray = Wevet::ComponentExtension::GetComponentsArray<UMeshComponent>(this);
	Super::CreateWeaponInstance(DefaultWeapon);
}

void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickableRecover(DeltaTime);
}

void AMockCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMockCharacter, MovementInput, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, RagdollLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, CharacterRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, LookingRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, TargetRotation, COND_SkipOwner);
}

// @NOTE
// 自動回復
void AMockCharacter::TickableRecover(const float InDeltaTime)
{
	if (IDamageInstigator::Execute_IsDeath(this))
	{
		return;
	}

	if (!IsFullHealth())
	{
		if (RecoverInterval >= RecoverTimer)
		{
			RecoverInterval = ZERO_VALUE;
			CharacterModel->Recover(RecoverHealthValue);
		}
		else
		{
			RecoverInterval += InDeltaTime;
		}
	}
}

void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects", IE_Pressed, this, &AMockCharacter::PickupObjects);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMockCharacter::ToggleEquip);
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMockCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMockCharacter::FireReleassed);
	PlayerInputComponent->BindAction("JumpAction", IE_Pressed, this, &AMockCharacter::Jump);
	PlayerInputComponent->BindAction("JumpAction", IE_Released, this, &AMockCharacter::StopJumping);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::Reload);

	// joystick Input
	PlayerInputComponent->BindAxis("LookRight", this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMockCharacter::LookUpAtRate);
}

#pragma region Input
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
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Dir, Value);
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
		const bool bCanNotClimbJumpLeft = (!Super::bCanClimbJumpLeft && Value < 0.f);
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
			const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Dir, Value);
		}
	}

}

void AMockCharacter::ReleaseObjects()
{
	Release_Implementation();
}

void AMockCharacter::PickupObjects()
{
	if (AActor* Actor = GetPickupComponent()->GetPickupActor())
	{
		Pickup_Implementation(IInteractionInstigator::Execute_GetItemType(Actor), Actor);
	}
}

void AMockCharacter::Jump()
{
	if (Super::bClimbJumping)
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Using ClimbSystem.. : %s"), *FString(__FUNCTION__));
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
		if (ALSStance == ELSStance::Crouching)
		{
			OnCrouch();
			return;
		}

		if (!IsPlayingRootMotion())
		{
			Super::Jump();
		}
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
		GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
	}
}

void AMockCharacter::UpdateWeapon()
{
	if (CurrentWeapon.IsValid())
	{
		if (CurrentWeapon->WasEquip())
		{
			return;
		}
	}

	const int32 WeaponNum = InventoryComponent->GetWeaponInventory().Num();
	if (WeaponCurrentIndex >= (WeaponNum - 1))
	{
		WeaponCurrentIndex = 0;
	}
	else
	{
		++WeaponCurrentIndex;
	}

	UE_LOG(LogWevetClient, Log, TEXT("CurrenIndex => %d, WeaponNum => %d"), WeaponCurrentIndex, WeaponNum);
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

#pragma region Interface
void AMockCharacter::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}

	Super::DisableInput(PlayerController);
	Super::Die_Implementation();
	//SetReplicateMovement(false);
	//TearOff();
	//bWasDied = true;
	//CurrentWeapon.Reset();
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SetRagdollPhysics();
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

	AAbstractWeapon* const WeaponPtr = InventoryComponent->FindByIndexWeapon(WeaponCurrentIndex);
	CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(WeaponPtr);
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
	IGrabInstigator::Execute_CanGrab(PlayerAnimInstance, bHanging);
	IGrabInstigator::Execute_ClimbLedge(PlayerAnimInstance, bClimbingLedge);
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

void AMockCharacter::TurnConerResult_Implementation()
{
	if (PlayerController)
	{
		Super::EnableInput(PlayerController);
	}
}

const bool AMockCharacter::CanPickup_Implementation()
{
	return true;
}

void AMockCharacter::Release_Implementation()
{
	const FVector ForwardOffset = Controller ? Controller->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (AAbstractWeapon* Weapon = Super::InventoryComponent->GetUnEquipWeapon())
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

	UE_LOG(LogWevetClient, Log, TEXT("ItemType : %s"), *GETENUMSTRING("EItemType", InItemType));

	switch (InItemType)
	{
	case EItemType::Weapon:
		if (AAbstractWeapon * Weapon = Cast<AAbstractWeapon>(Actor))
		{
			if (!Super::WasSameWeaponType(Weapon))
			{
				//const FWeaponItemInfo ItemInfo = Weapon->WeaponItemInfo;
				//IWeaponInstigator::Execute_DoReplenishment(Weapon, ItemInfo);
				Super::CreateWeaponInstance(Weapon->GetClass());
			}
			IInteractionInstigator::Execute_Release(Weapon, nullptr);
			Actor = nullptr;
		}
		break;
	case EItemType::Health:
		//
		break;
	case EItemType::Ammos:
		//
		break;
	}

}
#pragma endregion

void AMockCharacter::EquipmentActionMontage()
{
	check(InventoryComponent);

	if (WeaponCurrentIndex == INDEX_NONE)
	{
		UpdateWeapon();
	}

	AAbstractWeapon* const WeaponPtr = InventoryComponent->FindByIndexWeapon(WeaponCurrentIndex);
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

void AMockCharacter::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	for (UMeshComponent* Component : MeshArray)
	{
		if (Component)
		{
			Component->SetOwnerNoSee(NewOwnerNoSee);
		}
	}
	InventoryComponent->SetOwnerNoSeeMesh(NewOwnerNoSee);
}

#pragma region ALSInterface
void AMockCharacter::SetALSCharacterRotation_Implementation(const FRotator AddAmount)
{
	UE_LOG(LogWevetClient, Log, TEXT("Rotation : %s"), *FString(__FUNCTION__));
}

void AMockCharacter::SetALSCameraShake_Implementation(TSubclassOf<class UCameraShake> InShakeClass, const float InScale)
{
	if (PlayerController)
	{
		//PlayerController->ClientPlayCameraShake(InShakeClass, InScale, ECameraAnimPlaySpace::Type::CameraLocal);
	}
}
#pragma endregion

#pragma region ALS
// Ragdollを開始する
void AMockCharacter::StartRagdollAction()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
}

// Ragdollを終了する
void AMockCharacter::RagdollToWakeUpAction()
{
	GetCharacterMovement()->SetMovementMode(bRagdollOnGround ? EMovementMode::MOVE_Walking : EMovementMode::MOVE_Falling);
	GetCharacterMovement()->Velocity = RagdollVelocity;
	ILocomotionSystemAction::Execute_PoseSnapShot(GetAnimInstance(), RagdollPoseSnapshot);

	if (bRagdollOnGround)
	{
		const FRotator Rotation = GetMesh()->GetSocketRotation(PelvisBoneName);
		ILocomotionSystemAction::Execute_SetGetup(GetAnimInstance(), (Rotation.Roll > 0.0f));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}

// Ragdoll後はWeaponを開放する
void AMockCharacter::ReleaseWeaponInventory()
{
	const int32 WeaponNum = InventoryComponent->GetWeaponInventory().Num();
	const float Radius = DEFAULT_FORWARD_VECTOR;
	TArray<FVector> Points;
	UWevetBlueprintFunctionLibrary::CircleSpawnPoints(WeaponNum, Radius, GetActorLocation(), Points);

	int32 Index = 0;
	for (AAbstractWeapon* Weapon : InventoryComponent->GetWeaponInventory())
	{
		if (Weapon)
		{
			FTransform Transform = FTransform::Identity;
			Transform.SetLocation(Points[Index]);
			Super::ReleaseWeaponToWorld(Transform, Weapon);
			++Index;
		}
	}
	InventoryComponent->ClearWeaponInventory();
}

void AMockCharacter::DoWhileRagdoll()
{
	// 速度に基づいてラグドールの「固さ」を設定。
	// ラグドールの動きが速いほど、ジョイントの剛性が高くなる。
	const float Length = UKismetMathLibrary::VSize(ChooseVelocity());
	const float Value = UKismetMathLibrary::MapRangeClamped(Length, 0.0f, 1000.0f, 0.0f, 25000.0f);
	GetMesh()->SetAllMotorsAngularDriveParams(Value, 0.0f, 0.0f, false);

	// クライアントでローカルに制御されていないラグドールは、複製された「ラグドールの場所」ベクトルに向かってプッシュ。
	// これらは引き続き個別にシミュレーションされるが、同じ場所に配置される。
	if (!Super::IsLocallyControlled())
	{
		const FVector BoneLocation = GetMesh()->GetSocketLocation(PelvisBoneName);
		const FVector Position = (RagdollLocation - BoneLocation) * 200.0f;
		GetMesh()->AddForce(Position, PelvisBoneName, true);
	}
	else
	{
		//
	}
}

void AMockCharacter::CustomAcceleration()
{
	const auto Velocity = FMath::Abs(VelocityDifference);
	const float RangeA = 45.f;
	const float RangeB = 130.f;
	{
		auto ClampValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.2f);
		GetCharacterMovement()->MaxAcceleration = RunningAcceleration * ClampValue;
	}

	{
		auto ClampValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.4f);
		GetCharacterMovement()->GroundFriction = RunningGroundFriction * ClampValue;
	}
}

void AMockCharacter::AddCharacterRotation(const FRotator AddAmount)
{
	// Node to InvertRotator
	auto RotateAmount = UKismetMathLibrary::NegateRotator(AddAmount);
	TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, RotateAmount);
	auto RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;
	CharacterRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, RotateAmount);
	SetActorRotation(CharacterRotation);
}

void AMockCharacter::LimitRotation(const float AimYawLimit, const float InterpSpeed)
{
	if (FMath::Abs(AimYawDelta) > AimYawLimit)
	{
		bool bResult = (AimYawLimit > 0.0f);

		const float A = (LookingRotation.Yaw + AimYawLimit);
		const float B = (LookingRotation.Yaw - AimYawLimit);
		const float Value = bResult ? B : A;
		const FRotator Rotation = FRotator(0.f, Value, 0.f);
		ApplyCharacterRotation(Rotation, true, InterpSpeed);
	}
}

void AMockCharacter::ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed)
{
	TargetRotation = InTargetRotation;
	auto RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;

	if (bInterpRotation)
	{
		CharacterRotation = UKismetMathLibrary::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed);
	}
	else
	{
		CharacterRotation = TargetRotation;
	}
	SetActorRotation(CharacterRotation);
}

float AMockCharacter::ChooseMaxWalkSpeed() const
{
	float Speed = 0.0f;
	if (ALSStance == ELSStance::Standing)
	{
		if (bAiming)
		{
			switch (ALSGait)
			{
				case ELSGait::Walking:
				case ELSGait::Running:
					Speed = WalkingSpeed;
					break;
				case ELSGait::Sprinting:
					Speed = RunningSpeed;
					break;
			}
		}
		else
		{
			switch (ALSGait)
			{
				case ELSGait::Walking:
					Speed = WalkingSpeed;
					break;
				case ELSGait::Running:
					Speed = RunningSpeed;
					break;
				case ELSGait::Sprinting:
					Speed = SprintingSpeed;
					break;
			}
		}

	}
	else
	{
		const float Offset = 50.f;
		switch (ALSGait)
		{
			case ELSGait::Walking:
				Speed = CrouchingSpeed - Offset;
				break;
			case ELSGait::Running:
				Speed = CrouchingSpeed;
				break;
			case ELSGait::Sprinting:
				Speed = CrouchingSpeed + Offset;
				break;
		}
	}
	return Speed;
}

bool AMockCharacter::CanSprint() const
{
	if (ALSMovementMode == ELSMovementMode::Ragdoll)
	{
		return true;
	}
	else
	{
		if (ALSRotationMode == ELSRotationMode::VelocityDirection)
		{
			return true;
		}
		else
		{
			if (!bWasMovementInput)
			{
				return true;
			}

			if (bAiming)
			{
				return true;
			}
		}
	}
	const float YawLimit = 50.f;
	const FRotator Rot = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LookingRotation);
	return (FMath::Abs(Rot.Yaw) < YawLimit);
}

const float AMockCharacter::CalculateRotationRate(const float SlowSpeed, const float SlowSpeedRate, const float FastSpeed, const float FastSpeedRate)
{
	const FVector Velocity = ChooseVelocity();
	FVector Pos(Velocity.X, Velocity.Y, 0.0f);
	const float Size = UKismetMathLibrary::VSize(Pos);
	const float FastRange = UKismetMathLibrary::MapRangeClamped(Size, SlowSpeed, FastSpeed, SlowSpeedRate, FastSpeedRate);
	const float SlowRange = UKismetMathLibrary::MapRangeClamped(Size, 0.0f, SlowSpeed, 0.0f, SlowSpeedRate);

	if (RotationRateMultiplier != 1.0f)
	{
		RotationRateMultiplier = FMath::Clamp(RotationRateMultiplier + GetWorld()->GetDeltaSeconds(), 0.0f, 1.0f);
	}
	const float Value = (Size > SlowSpeed) ? FastRange : SlowRange;
	auto Result = Value * RotationRateMultiplier;
	const float Min = 0.1f;
	const float Max = 15.0f;
	return FMath::Clamp(Result, Min, Max);
}

#pragma endregion
