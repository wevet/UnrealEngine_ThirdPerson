// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockCharacter.h"
#include "Player/MockPlayerController.h"
#include "AnimInstance/PlayerAnimInstance.h"
#include "AnimInstance/IKAnimInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/RotationMatrix.h"

#include "Wevet.h"
#include "WevetExtension.h"
#include "Net/UnrealNetwork.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
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

	GetMesh()->ComponentTags.Add(WATER_TAG);

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

	WeaponCurrentIndex = 0;
	RecoverHealthValue = 100;
	RecoverTimer = 2.0f;
	Tags.Add(DAMAGE_TAG);

	// ALS
	bWasMovementInput = false;
	bWasMoving = false;
	bAiming = false;
	bRightShoulder = true;
	bRagdollOnGround = false;

	// ALS Override
	GetCharacterMovement()->BrakingFrictionFactor = ZERO_VALUE;
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;

	// FPSSocketRotation
	FPSSocketRotation = FRotator(0.f, 90.0f, -90.f);

	// ItemPickupEffect
	OutlinePostProcessComponent = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("OutlinePostProcessComponent"));
	OutlinePostProcessComponent->bAutoActivate = 1;
	OutlinePostProcessComponent->bEnabled = 0;
	OutlinePostProcessComponent->bUnbound = 0;
	OutlinePostProcessComponent->BlendWeight = 0.0f;
	OutlinePostProcessComponent->SetVisibility(false);
	OutlinePostProcessComponent->SetupAttachment(GetCapsuleComponent());

	// Ž€–SŽžEffect
	DeathPostProcessComponent = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("DeathPostProcessComponent"));
	DeathPostProcessComponent->bAutoActivate = 1;
	DeathPostProcessComponent->bEnabled = 0;
	DeathPostProcessComponent->bUnbound = 0;
	DeathPostProcessComponent->BlendWeight = 0.0f;
	DeathPostProcessComponent->SetVisibility(false);
	DeathPostProcessComponent->SetupAttachment(GetCapsuleComponent());

	//wchar_t* Path = TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/CameraLerpCurves/");
	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> FindAsset(TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/CameraLerpCurves/DefaultCurve"));
		DefaultCurve = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> FindAsset(TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/CameraLerpCurves/FastCurve"));
		FastCurve = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> FindAsset(TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/CameraLerpCurves/SlowCurve"));
		SlowCurve = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> FindAsset(TEXT("/Game/Player_Assets/Character/Animation/Extra/Mantle/ALS_N_Mantle_1m_RH_Montage"));
		DefaultLowMantleAsset.AnimMontage = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> FindAsset(TEXT("/Game/Player_Assets/Character/Animation/Extra/Mantle/ALS_N_Mantle_2m_Montage"));
		DefaultHighMantleAsset.AnimMontage = FindAsset.Object;
	}
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
	FPSCameraComponent->SetRelativeRotation(FPSSocketRotation);
}

void AMockCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MeshArray.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void AMockCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AMockPlayerController>(GetController());
	PlayerAnimInstance = Cast<UPlayerAnimInstance>(GetAnimInstance());
	MeshArray = Wevet::ComponentExtension::GetComponentsArray<UMeshComponent>(this);
	ILocomotionSystemPawn::Execute_OnALSViewModeChange(this);
	ILocomotionSystemPawn::Execute_OnALSRotationModeChange(this);
	ILocomotionSystemPawn::Execute_OnALSStanceChange(this);

	Super::CreateWeaponInstance(PrimaryWeapon, [&](AAbstractWeapon* Weapon)
	{
		if (Weapon)
		{
			UE_LOG(LogWevetClient, Log, TEXT("Primary Weapon Instance Success : %s"), *FString(__FUNCTION__));
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("Primary Weapon Instance Failer : %s"), *FString(__FUNCTION__));
		}
	});
	Super::CreateWeaponInstance(SecondaryWeapon, [&](AAbstractWeapon* Weapon) 
	{
		if (Weapon)
		{
			UE_LOG(LogWevetClient, Log, TEXT("Secondary Weapon Instance Success : %s"), *FString(__FUNCTION__));
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("Secondary Weapon Instance Failer : %s"), *FString(__FUNCTION__));
		}
	});
}

void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickableRecover(DeltaTime);
	CalculateEssentialVariables();
}

void AMockCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMockCharacter, MovementInput, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, RagdollLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, CharacterRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMockCharacter, LookingRotation, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AMockCharacter, TargetRotation, COND_SkipOwner);
}

void AMockCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, GetPawnMovementModeChanged(PrevMovementMode, PreviousCustomMode));
}

void AMockCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	ILocomotionSystemPawn::Execute_SetALSStance(this, ELSStance::Crouching);
}

void AMockCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	ILocomotionSystemPawn::Execute_SetALSStance(this, ELSStance::Standing);
}

void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects", IE_Pressed, this, &AMockCharacter::PickupObjects);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMockCharacter::ToggleEquip);
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::Reload);

	// Crouch
	PlayerInputComponent->BindAction("CrouchAction", IE_Pressed, this, &AMockCharacter::OnCrouch);

	// FirePress
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMockCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMockCharacter::FireReleassed);

	// MeleeAttack
	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &AMockCharacter::MeleeAttack);

	// Jump
	PlayerInputComponent->BindAction("JumpAction", IE_Pressed, this, &AMockCharacter::Jump);
	PlayerInputComponent->BindAction("JumpAction", IE_Released, this, &AMockCharacter::StopJumping);

	// Sprint
	PlayerInputComponent->BindAction("SprintAction", IE_Pressed, this, &AMockCharacter::Sprint);
	PlayerInputComponent->BindAction("SprintAction", IE_Released, this, &AMockCharacter::StopSprint);

	// Aiming
	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &AMockCharacter::Aiming);
	//PlayerInputComponent->BindAction("AimAction", IE_Released, this, &AMockCharacter::StopAiming);

	// joystick Input
	PlayerInputComponent->BindAxis("LookRight", this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMockCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMockCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMockCharacter::MoveRight);
}

#pragma region Input
void AMockCharacter::TurnAtRate(float Rate)
{
	Super::TurnAtRate(Rate);
}

void AMockCharacter::LookUpAtRate(float Rate)
{
	Super::LookUpAtRate(Rate);
}

void AMockCharacter::MoveForward(float Value)
{
	ForwardAxisValue = Value;
	PlayerMovementInput(true);
}

void AMockCharacter::MoveRight(float Value)
{
	RightAxisValue = Value;
	PlayerMovementInput(false);
}

void AMockCharacter::Aiming()
{
	if (bAiming)
	{
		StopAiming();
		return;
	}

	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ELSRotationMode::LookingDirection);
		break;
		case ELSRotationMode::LookingDirection:
		break;
	}
	ILocomotionSystemPawn::Execute_SetALSAiming(this, true);
}

void AMockCharacter::StopAiming()
{
	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		break;
		case ELSRotationMode::LookingDirection:
		ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ELSRotationMode::VelocityDirection);
		break;
	}
	ILocomotionSystemPawn::Execute_SetALSAiming(this, false);
}

void AMockCharacter::PlayerMovementInput(const bool bForwardAxis)
{
	switch (ALSMovementMode)
	{
		case ELSMovementMode::Grounded:
		case ELSMovementMode::Swimming:
		case ELSMovementMode::Falling:
		GroundMovementInput(bForwardAxis);
		break;
		case ELSMovementMode::Ragdoll:
		RagdollMovementInput();
		break;
	}
}

void AMockCharacter::GroundMovementInput(const bool bForwardAxis)
{
	FVector OutForwardVector;
	FVector OutRightVector;
	Super::SetForwardOrRightVector(OutForwardVector, OutRightVector);

	if (bForwardAxis)
	{
		AddMovementInput(OutForwardVector, ForwardAxisValue);
	}
	else
	{
		AddMovementInput(OutRightVector, RightAxisValue);
	}
}

void AMockCharacter::RagdollMovementInput()
{
	FVector OutForwardVector;
	FVector OutRightVector;
	Super::SetForwardOrRightVector(OutForwardVector, OutRightVector);
	const FVector Position = UKismetMathLibrary::Normal((OutForwardVector * ForwardAxisValue) + (OutRightVector * RightAxisValue));

	float Speed = 0.0f;
	switch (ALSGait)
	{
		case ELSGait::Walking:
		case ELSGait::Running:
		Speed = 500.f;
		break;
		case ELSGait::Sprinting:
		Speed = 800.f;
		break;
	}

	const FVector Torque = Position * Speed;
	GetMesh()->AddTorqueInRadians(FVector(Torque.X * -1.f, Torque.Y, Torque.Z), PelvisBoneName, true);
	GetCharacterMovement()->AddInputVector(Position);
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
	if (ALSMovementAction != ELSMovementAction::None)
	{
		return;
	}

	switch (ALSMovementMode)
	{
		case ELSMovementMode::Grounded:
		{
			bool bWasMantleFail = false;
			if (bWasMovementInput)
			{
				bWasMantleFail = (!MantleCheck(GroundedTraceSettings));
			}
			else
			{
				bWasMantleFail = true;
			}

			if (bWasMantleFail)
			{
				switch (ALSStance)
				{
					case ELSStance::Standing:
					Super::Jump();
					break;
					case ELSStance::Crouching:
					OnCrouch();
					break;
				}
			}
		}
		break;
		case ELSMovementMode::Falling:
		{
			MantleCheck(FallingTraceSettings);
		}
		break;
		case ELSMovementMode::Mantling:
		break;
		case ELSMovementMode::None:
		break;
		case ELSMovementMode::Ragdoll:
		break;
		case ELSMovementMode::Swimming:
		break;
	}

}

void AMockCharacter::StopJumping()
{
	Super::StopJumping();
}

void AMockCharacter::Sprint()
{
	Super::Sprint();

	switch (ALSGait)
	{
		case ELSGait::Walking:
		ILocomotionSystemPawn::Execute_SetALSGait(this, ELSGait::Running);
		break;
		case ELSGait::Running:
		break;
		case ELSGait::Sprinting:
		break;
	}
}

void AMockCharacter::StopSprint()
{
	Super::StopSprint();
}

void AMockCharacter::FirePressed()
{
	Super::FirePressed();
}

void AMockCharacter::FireReleassed()
{
	Super::FireReleassed();
}

void AMockCharacter::MeleeAttack()
{
	Super::MeleeAttack();
}

void AMockCharacter::Reload()
{
	Super::Reload();
}

void AMockCharacter::OnCrouch()
{
	if (ALSMovementMode == ELSMovementMode::Grounded)
	{
		Super::OnCrouch();
		if (Super::bCrouch)
		{
			Super::bSprint = false;
			GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
		}
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

	if (InventoryComponent->HasInventoryWeapon())
	{
		return;
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
	//UE_LOG(LogWevetClient, Log, TEXT("CurrenIndex => %d, WeaponNum => %d"), WeaponCurrentIndex, WeaponNum);
}

void AMockCharacter::ToggleEquip()
{
	if (CurrentWeapon.IsValid())
	{
		UnEquipmentActionMontage();
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
	if (!IDamageInstigator::Execute_IsDeath(this))
	{
		CharacterModel->TakeDamage(CharacterModel->GetMaxHealth());
		CharacterModel->Die();
	}

	if (Super::bWasDied)
	{
		return;
	}

	Super::bWasDied = true;
	if (DeathDelegate.IsBound())
	{
		DeathDelegate.Broadcast();
	}

	if (PlayerAnimInstance)
	{
		PlayerAnimInstance->SetArmTorsoIKMode(false, false);
	}

	StopAiming();
	ReleaseAllWeaponInventory();
	VisibleDeathPostProcess(true);
	//Super::DisableInput(PlayerController);
}

void AMockCharacter::Alive_Implementation()
{
	Super::Alive_Implementation();
	Super::bWasDied = false;

	if (AliveDelegate.IsBound())
	{
		AliveDelegate.Broadcast();
	}
	VisibleDeathPostProcess(false);
	//Super::EnableInput(PlayerController);
}

void AMockCharacter::Equipment_Implementation()
{
	AAbstractWeapon* const WeaponPtr = InventoryComponent->FindByIndexWeapon(WeaponCurrentIndex);
	CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(WeaponPtr);
	if (!CurrentWeapon.IsValid())
	{
		return;
	}
	Super::Equipment_Implementation();
}

void AMockCharacter::UnEquipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
		return;

	Super::UnEquipment_Implementation();
	CurrentWeapon.Reset();
}

const bool AMockCharacter::CanPickup_Implementation()
{
	return !IDamageInstigator::Execute_IsDeath(this);
}

void AMockCharacter::Release_Implementation()
{
	const FVector ForwardOffset = PlayerController ? PlayerController->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (AAbstractWeapon* Weapon = InventoryComponent->GetUnEquipWeaponByIndex(WeaponCurrentIndex))
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
		{
			if (AAbstractWeapon* Weapon = Cast<AAbstractWeapon>(Actor))
			{
				if (!Super::WasSameWeaponType(Weapon))
				{
					FWeaponItemInfo WeaponItemInfo = Weapon->GetWeaponItemInfo();
					Super::CreateWeaponInstance(Weapon->GetTemplateClass(), [&](AAbstractWeapon* InWeapon) 
					{
						if (InWeapon)
						{
							InWeapon->CopyWeaponItemInfo(&WeaponItemInfo);
						}
					});
					IInteractionInstigator::Execute_Release(Weapon, this);
					Actor = nullptr;
				}
				else
				{
					//SameWeaponType
				}
			}
		}
		break;

		case EItemType::Health:
		{
			//
		}
		break;

		case EItemType::Ammos:
		{
			if (AAbstractItem* Item = Cast<AAbstractItem>(Actor))
			{
				if (AAbstractWeapon* Weapon = FindByWeapon(Item->GetWeaponItemType()))
				{
					IWeaponInstigator::Execute_DoReplenishment(Weapon, Item->GetReplenishmentAmmo());
					IInteractionInstigator::Execute_Release(Item, this);
					Actor = nullptr;
				}
			}
		}
		break;
	}

}
#pragma endregion

#pragma region Montages
void AMockCharacter::EquipmentActionMontage()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	check(InventoryComponent);

	AAbstractWeapon* const WeaponPtr = InventoryComponent->FindByIndexWeapon(WeaponCurrentIndex);
	if (WeaponPtr)
	{
		SetActionInfo(WeaponPtr->GetWeaponItemInfo().WeaponItemType);
		if (ActionInfoPtr && ActionInfoPtr->EquipMontage)
		{
			PlayAnimMontage(ActionInfoPtr->EquipMontage, MONTAGE_DELAY);
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
#pragma endregion

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

void AMockCharacter::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	for (UMeshComponent* Component : MeshArray)
	{
		if (Component)
			Component->SetOwnerNoSee(NewOwnerNoSee);
	}
	InventoryComponent->SetOwnerNoSeeMesh(NewOwnerNoSee);
}

void AMockCharacter::VisibleDeathPostProcess(const bool InEnabled)
{
	if (DeathPostProcessComponent)
	{
		DeathPostProcessComponent->bEnabled = InEnabled ? 1 : 0;
		DeathPostProcessComponent->bUnbound = InEnabled ? 1 : 0;
		DeathPostProcessComponent->BlendWeight = InEnabled ? 1.0f : 0.0f;
		DeathPostProcessComponent->SetVisibility(InEnabled);
	}
}

#pragma region ALSInterface
void AMockCharacter::SetALSCharacterRotation_Implementation(const FRotator AddAmount)
{
	//UE_LOG(LogWevetClient, Log, TEXT("Rotation : %s"), *FString(__FUNCTION__));
}

void AMockCharacter::SetALSCameraShake_Implementation(TSubclassOf<class UCameraShake> InShakeClass, const float InScale)
{
	if (PlayerController)
	{
		//PlayerController->ClientPlayCameraShake(InShakeClass, InScale, ECameraAnimPlaySpace::Type::CameraLocal);
	}
}

void AMockCharacter::SetALSMovementMode_Implementation(const ELSMovementMode NewALSMovementMode)
{
	if (ALSMovementMode == NewALSMovementMode)
	{
		return;
	}
	ALSPrevMovementMode = ALSMovementMode;
	ALSMovementMode = NewALSMovementMode;
	ILocomotionSystemPawn::Execute_OnALSMovementModeChange(this);
}

void AMockCharacter::OnALSMovementModeChange_Implementation()
{
	ILocomotionSystemState::Execute_SetALSMovementMode(GetAnimInstance(), ALSMovementMode);
	//ILocomotionSystemState::Execute_SetALSMovementMode(GetIKAnimInstance(), ALSMovementMode);
	UpdateCharacterMovementSettings();
}

void AMockCharacter::SetALSMovementAction_Implementation(const ELSMovementAction NewALSMovementAction)
{
	if (ALSMovementAction == NewALSMovementAction)
	{
		return;
	}
	ALSPrevMovementAction = ALSMovementAction;
	ALSMovementAction = NewALSMovementAction;
	ILocomotionSystemPawn::Execute_OnALSMovementActionChange(this);
}

void AMockCharacter::OnALSMovementActionChange_Implementation()
{
	//
}

void AMockCharacter::SetALSGait_Implementation(const ELSGait NewALSGait)
{
	if (ALSGait == NewALSGait)
	{
		return;
	}
	ALSGait = NewALSGait;
}

void AMockCharacter::OnALSGaitChange_Implementation()
{
	ILocomotionSystemState::Execute_SetALSGait(GetAnimInstance(), ALSGait);
	UpdateCharacterMovementSettings();
	if (Super::IsLocallyControlled())
	{
		BP_UpdateCameraAction(bAiming ? FastCurve : SlowCurve);
	}
}

void AMockCharacter::SetALSStance_Implementation(const ELSStance NewALSStance)
{
	if (ALSStance == NewALSStance)
	{
		return;
	}
	ALSStance = NewALSStance;
	ILocomotionSystemPawn::Execute_OnALSStanceChange(this);
}

void AMockCharacter::OnALSStanceChange_Implementation()
{
	ILocomotionSystemState::Execute_SetALSStance(GetAnimInstance(), ALSStance);
	//ILocomotionSystemState::Execute_SetALSStance(GetIKAnimInstance(), ALSStance);
	UpdateCharacterMovementSettings();
	BP_UpdateCameraAction(SlowCurve);
}

void AMockCharacter::SetALSRotationMode_Implementation(const ELSRotationMode NewALSRotationMode)
{
	if (ALSRotationMode == NewALSRotationMode)
	{
		return;
	}
	ALSRotationMode = NewALSRotationMode;
}

void AMockCharacter::OnALSRotationModeChange_Implementation()
{
	ILocomotionSystemState::Execute_SetALSRotationMode(GetAnimInstance(), ALSRotationMode);
	BP_UpdateCameraAction(DefaultCurve);
	if (bWasMoving)
	{
		RotationRateMultiplier = 0.0f;
	}

	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		{
			switch (ALSViewMode)
			{
				case ELSViewMode::FirstPerson:
				ILocomotionSystemPawn::Execute_SetALSViewMode(this, ELSViewMode::ThirdPerson);
				break;
				case ELSViewMode::ThirdPerson:
				break;
			}
		}
		break;
		case ELSRotationMode::LookingDirection:
		{
		}
		break;
	}
}

void AMockCharacter::SetALSViewMode_Implementation(const ELSViewMode NewALSViewMode)
{
	if (ALSViewMode == NewALSViewMode)
	{
		return;
	}
	ALSViewMode = NewALSViewMode;
	ILocomotionSystemPawn::Execute_OnALSViewModeChange(this);
}

void AMockCharacter::OnALSViewModeChange_Implementation()
{
	ILocomotionSystemState::Execute_SetALSViewMode(GetAnimInstance(), ALSViewMode);
	switch (ALSViewMode)
	{
		case ELSViewMode::ThirdPerson:
		{
			FPSCameraComponent->SetActive(false);
			TPSCameraComponent->SetActive(true);
		}
		break;
		case ELSViewMode::FirstPerson:
		{
			TPSCameraComponent->SetActive(false);
			FPSCameraComponent->SetActive(true);
			ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ELSRotationMode::LookingDirection);
		}
		break;
	}
}

void AMockCharacter::SetALSAiming_Implementation(const bool NewALSAiming)
{
	if (bAiming == NewALSAiming)
	{
		return;
	}
	bAiming = NewALSAiming;
}

void AMockCharacter::OnALSAimingChange_Implementation()
{
	ILocomotionSystemAction::Execute_SetAiming(GetAnimInstance(), bAiming);
	UpdateCharacterMovementSettings();
	BP_UpdateCameraAction(bAiming ? FastCurve : DefaultCurve);
}
#pragma endregion

#pragma region ALSRagdoll
void AMockCharacter::StartRagdollAction()
{
	UnEquipment_Implementation();
	Super::SetReplicateMovement(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, ELSMovementMode::Ragdoll);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
}

void AMockCharacter::RagdollToWakeUpAction()
{
	Super::SetReplicateMovement(true);
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

void AMockCharacter::DoWhileRagdoll(FRotator& OutActorRotation, FVector& OutActorLocation)
{
	// Set the "stiffness" of the ragdoll based on the speed.
	// The faster the ragdoll moves, the more rigid the joint.
	const float Length = UKismetMathLibrary::VSize(ChooseVelocity());
	const float Value = UKismetMathLibrary::MapRangeClamped(Length, 0.0f, 1000.0f, 0.0f, 25000.0f);
	GetMesh()->SetAllMotorsAngularDriveParams(Value, 0.0f, 0.0f, false);

	// Ragdolls that are not locally controlled by the client are pushed towards the duplicated "Ragdoll Location" vector.
	// These will continue to be simulated individually, but will be co-located.
	if (!Super::IsLocallyControlled())
	{
		const FVector BoneLocation = GetMesh()->GetSocketLocation(PelvisBoneName);
		const FVector Position = (RagdollLocation - BoneLocation) * 200.0f;
		GetMesh()->AddForce(Position, PelvisBoneName, true);
		return;
	}

	// If the fall is too fast, disable gravity to prevent the ragdoll from continuing to accelerate.
	// Stabilize the movement of the Ragdoll and prevent it from falling off the floor.
	GetMesh()->SetEnableGravity((ChooseVelocity().Z < -4000.f) ? false : true);

	RagdollVelocity = ChooseVelocity();
	RagdollLocation = GetMesh()->GetSocketLocation(PelvisBoneName);
	const FRotator BoneRotation = GetMesh()->GetSocketRotation(PelvisBoneName);

	CalculateActorTransformRagdoll(BoneRotation, RagdollLocation, OutActorRotation, OutActorLocation);
	SetActorLocation(OutActorLocation);

	TargetRotation = OutActorRotation;
	RotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation).Yaw;
	CharacterRotation = OutActorRotation;
	SetActorRotation(CharacterRotation);
}

void AMockCharacter::CalculateActorTransformRagdoll(const FRotator InRagdollRotation, const FVector InRagdollLocation, FRotator& OutActorRotation, FVector& OutActorLocation)
{
	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector StartLocation(InRagdollLocation);
	const FVector EndLocation(InRagdollLocation.X, InRagdollLocation.Y, InRagdollLocation.Z - CapsuleHalfHeight);
	FHitResult HitResult;

	const bool bResult = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		IgnoreActors,
		bDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		HitResult,
		true);

	bRagdollOnGround = HitResult.bBlockingHit;
	const float Offset = 2.0f;
	const float Diff = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
	const float Value = bRagdollOnGround ? (CapsuleHalfHeight - Diff) + Offset : 0.0f;
	OutActorLocation = FVector(InRagdollLocation.X, InRagdollLocation.Y, InRagdollLocation.Z + Value);

	const float Yaw = (OutActorRotation.Roll > 0.0f) ? OutActorRotation.Yaw : OutActorRotation.Yaw - 180.f;
	OutActorRotation = FRotator(0.0f, Yaw, 0.0f);
}
#pragma endregion

#pragma region ALSUpdate
void AMockCharacter::CalculateEssentialVariables()
{
	// Check if the Character is moving and set (last speed rotation) and (direction) only when it is moving .
	// so that they do not return to 0 when the speed is 0.
	{
		const FVector Velocity(ChooseVelocity().X, ChooseVelocity().Y, 0.0f);
		bWasMoving = UKismetMathLibrary::NotEqual_VectorVector(Velocity, FVector::ZeroVector, 1.0f);

		if (bWasMoving)
		{
			LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(ChooseVelocity());
			const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, CharacterRotation);
			Direction = DeltaRot.Yaw;
		}
	}

	// Set MovementInput to local, send to server and duplicate (only if Game is NW connected)
	{
		if (Super::IsLocallyControlled())
		{
			MovementInput = GetCharacterMovement()->GetLastInputVector();
			bWasMovementInput = UKismetMathLibrary::NotEqual_VectorVector(MovementInput, FVector::ZeroVector, 0.0001f);

			if (bWasMovementInput)
			{
				LastMovementInputRotation = UKismetMathLibrary::Conv_VectorToRotator(MovementInput);
				const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LastVelocityRotation);
				VelocityDifference = DeltaRot.Yaw;
			}
		}
	}

	// Set LookRotation to local and send to server to duplicate (only if the game is connected to the network)
	{
		if (Super::IsLocallyControlled())
		{
			const float PrevAimYaw = LookingRotation.Yaw;
			LookingRotation = GetControlRotation();

			const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
			AimYawRate = (LookingRotation.Yaw - PrevAimYaw) / DeltaSeconds;
		}

		const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, CharacterRotation);
		AimYawDelta = DeltaRot.Yaw;
	}
}

void AMockCharacter::ManageCharacterRotation()
{
	if (Super::IsLocallyControlled())
	{
		switch (ALSMovementMode)
		{
			case ELSMovementMode::Grounded:
			case ELSMovementMode::Swimming:
			DoCharacterGrounded();
			break;
			case ELSMovementMode::Falling:
			DoCharacterFalling();
			break;
			case ELSMovementMode::Ragdoll:
			break;
		}
	}
}

void AMockCharacter::DoCharacterFalling()
{
	const float InterpSpeed = 10.0f;

	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		if (bWasMoving)
		{
			ApplyCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, InterpSpeed);
		}
		break;
		case ELSRotationMode::LookingDirection:
		{
			JumpRotation = LookingRotation;
			ApplyCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, InterpSpeed);
		}
		break;
	}
}

void AMockCharacter::DoCharacterGrounded()
{
	if (!bWasMoving)
	{
		if (!Super::IsPlayingRootMotion())
		{
			if (ALSRotationMode == ELSRotationMode::LookingDirection)
			{
				if (bAiming || ALSViewMode == ELSViewMode::FirstPerson)
				{
					LimitRotation(90.f, 15.f);
				}
			}
		}
		return;
	}

	// Moving
	const FRotator Rotation = LookingDirectionWithOffset(5.f, 60.f, -60.f, 120.f, -120.f, 5.f);
	const float SlowSpeed = 165.f;
	const float FastSpeed = 375.f;
	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		{
			// use last speed rotation
			const float RotationRate = CalculateRotationRate(SlowSpeed, 5.f, FastSpeed, 10.f);
			ApplyCharacterRotation(FRotator(0.0f, LastVelocityRotation.Yaw, 0.0f), true, RotationRate);
		}
		break;

		case ELSRotationMode::LookingDirection:
		{
			float RotationRate = 0.0f;
			if (bAiming)
			{
				RotationRate = CalculateRotationRate(SlowSpeed, 15.f, FastSpeed, 15.f);
			}
			else
			{
				RotationRate = CalculateRotationRate(SlowSpeed, 10.f, FastSpeed, 15.f);
			}

			ApplyCharacterRotation(Rotation, true, RotationRate);
		}
		break;
	}
}

void AMockCharacter::DoWhileGrounded()
{
	bool bWasStanding = false;
	switch (ALSStance)
	{
		case ELSStance::Standing:
		bWasStanding = true;
		break;
		case ELSStance::Crouching:
		break;
	}

	if (!bWasStanding)
	{
		return;
	}

	switch (ALSGait)
	{
		case ELSGait::Walking:
		break;
		case ELSGait::Running:
		case ELSGait::Sprinting:
		CustomAcceleration();
		break;
	}
}
#pragma endregion

#pragma region ALS
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

const FRotator AMockCharacter::LookingDirectionWithOffset(const float OffsetInterpSpeed, const float NEAngle, const float NWAngle, const float SEAngle, const float SWAngle, const float Buffer)
{
	const FRotator LastRotation = bWasMovementInput ? LastMovementInputRotation : LastVelocityRotation;
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastRotation, LookingRotation);

	if (CardinalDirectionAngles(DeltaRot.Yaw, NWAngle, NEAngle, Buffer, ELSCardinalDirection::North))
	{
		CardinalDirection = ELSCardinalDirection::North;
	}
	else if (CardinalDirectionAngles(DeltaRot.Yaw, NEAngle, SEAngle, Buffer, ELSCardinalDirection::East))
	{
		CardinalDirection = ELSCardinalDirection::East;
	}
	else if (CardinalDirectionAngles(DeltaRot.Yaw, SWAngle, NWAngle, Buffer, ELSCardinalDirection::West))
	{
		CardinalDirection = ELSCardinalDirection::West;
	}
	else
	{
		CardinalDirection = ELSCardinalDirection::South;
	}

	float Result = 0.0f;
	switch (CardinalDirection)
	{
		case ELSCardinalDirection::North:
		Result = DeltaRot.Yaw;
		break;
		case ELSCardinalDirection::East:
		Result = (DeltaRot.Yaw - 90.f);
		break;
		case ELSCardinalDirection::West:
		Result = (DeltaRot.Yaw + 90.f);
		break;
		case ELSCardinalDirection::South:
		Result = UKismetMathLibrary::SelectFloat((DeltaRot.Yaw - 180.f), (DeltaRot.Yaw + 180.f), (DeltaRot.Yaw > 0.0f));
		break;
	}

	if (bAiming)
	{
		if (ALSGait == ELSGait::Walking)
		{
			Result = 0.0f;
		}
	}
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	RotationOffset = UKismetMathLibrary::FInterpTo(RotationOffset, Result, DeltaSeconds, OffsetInterpSpeed);
	return FRotator(0.0f, LookingRotation.Yaw + RotationOffset, 0.0f);
}

void AMockCharacter::UpdateCharacterMovementSettings()
{
	GetCharacterMovement()->MaxWalkSpeed = ChooseMaxWalkSpeed();
	GetCharacterMovement()->MaxWalkSpeedCrouched = ChooseMaxWalkSpeed();

	GetCharacterMovement()->MaxAcceleration = ChooseMaxAcceleration();
	GetCharacterMovement()->BrakingDecelerationWalking = ChooseBrakingDeceleration();
	GetCharacterMovement()->GroundFriction = ChooseGroundFriction();
}

void AMockCharacter::ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed)
{
	TargetRotation = InTargetRotation;
	const FRotator RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;
	CharacterRotation = bInterpRotation ? 
		UKismetMathLibrary::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed) : TargetRotation;
	SetActorRotation(CharacterRotation);
}

void AMockCharacter::LimitRotation(const float AimYawLimit, const float InterpSpeed)
{
	if (FMath::Abs(AimYawDelta) > AimYawLimit)
	{
		const float A = (LookingRotation.Yaw + AimYawLimit);
		const float B = (LookingRotation.Yaw - AimYawLimit);
		const float Value = (AimYawLimit > 0.0f) ? B : A;
		const FRotator Rotation = FRotator(0.f, Value, 0.f);
		ApplyCharacterRotation(Rotation, true, InterpSpeed);
	}
}

bool AMockCharacter::CardinalDirectionAngles(const float Value, const float Min, const float Max, const float Buffer, const ELSCardinalDirection InCardinalDirection) const
{
	const bool A = UKismetMathLibrary::InRange_FloatFloat(Value, (Min + Buffer), (Max - Buffer));
	const bool B = UKismetMathLibrary::InRange_FloatFloat(Value, (Min - Buffer), (Max + Buffer));
	return (CardinalDirection == InCardinalDirection) ? B : A;
}

void AMockCharacter::CustomAcceleration()
{
	const auto Velocity = FMath::Abs(VelocityDifference);
	const float RangeA = 45.f;
	const float RangeB = 130.f;

	auto MaxAccelerationValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.2f);
	GetCharacterMovement()->MaxAcceleration = RunningAcceleration * MaxAccelerationValue;
	auto GroundFrictionValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.4f);
	GetCharacterMovement()->GroundFriction = RunningGroundFriction * GroundFrictionValue;
}
#pragma endregion

