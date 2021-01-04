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

	bReplicates = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraTraceLSocket = CAMERA_TRACE_L;
	CameraTraceRSocket = CAMERA_TRACE_R;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.1f;

	GetMesh()->ComponentTags.Add(WATER_TAG);

	bEnableRagdoll = false;
	JumpMaxHoldTime = 0.5f;
	BaseTurnRate = 150.f;
	BaseLookUpRate = 150.f;

	WeaponCurrentIndex = 0;
	RecoverHealthValue = 100;
	RecoverTimer = 2.0f;
	Tags.Add(DAMAGE_TAG);
	Tags.Add(PLAYER_TAG);

	// ALS
	bWasMovementInput = false;
	bRagdollOnGround = false;

	// ItemPickupEffect
	OutlinePostProcessComponent = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("OutlinePostProcessComponent"));
	OutlinePostProcessComponent->bAutoActivate = 1;
	OutlinePostProcessComponent->bEnabled = 0;
	OutlinePostProcessComponent->bUnbound = 0;
	OutlinePostProcessComponent->BlendWeight = 0.0f;
	OutlinePostProcessComponent->SetVisibility(false);
	OutlinePostProcessComponent->SetupAttachment(GetCapsuleComponent());

	// Death PostProcessEffect
	DeathPostProcessComponent = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("DeathPostProcessComponent"));
	DeathPostProcessComponent->bAutoActivate = 1;
	DeathPostProcessComponent->bEnabled = 0;
	DeathPostProcessComponent->bUnbound = 0;
	DeathPostProcessComponent->BlendWeight = 0.0f;
	DeathPostProcessComponent->SetVisibility(false);
	DeathPostProcessComponent->SetupAttachment(GetCapsuleComponent());

	// BackPack
	static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Tool/BP_Backpack.BP_Backpack_C"));
	BackPackTemplate = FindAsset.Object;

	// TeamID = 2

	// LookingDirection Initialize
	ALSRotationMode = ELSRotationMode::LookingDirection;
}

void AMockCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
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

	{
		SpawnBackPack();
	}

	CreateWeaponInstance(PrimaryWeapon, [&](AAbstractWeapon* Weapon)
	{
	});

	CreateWeaponInstance(SecondaryWeapon, [&](AAbstractWeapon* Weapon) 
	{
	});
}

void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickableRecover(DeltaTime);
}

void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects", IE_Pressed, this, &AMockCharacter::PickupObjects);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMockCharacter::ToggleEquip);
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &AMockCharacter::UpdateWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMockCharacter::OnReload);

	// Crouch
	PlayerInputComponent->BindAction("CrouchAction", IE_Pressed, this, &AMockCharacter::OnCrouch);

	// FirePress
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMockCharacter::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMockCharacter::OnFireReleassed);

	// MeleeAttack
	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &AMockCharacter::OnMeleeAttack);

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
void AMockCharacter::OnFirePressed()
{
	Super::DoFirePressed_Implementation();
}

void AMockCharacter::OnFireReleassed()
{
	Super::DoFireReleassed_Implementation();
}

void AMockCharacter::OnMeleeAttack()
{
	Super::DoMeleeAttack_Implementation();
}

void AMockCharacter::OnReload()
{
	Super::DoReload_Implementation();
}

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
		{
			//ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ELSRotationMode::LookingDirection);
		}
		break;
		case ELSRotationMode::LookingDirection:
		{
			//
		}
		break;
	}
	ILocomotionSystemPawn::Execute_SetALSAiming(this, true);
}

void AMockCharacter::StopAiming()
{
	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		{
			//
		}
		break;
		case ELSRotationMode::LookingDirection:
		{
			//ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ELSRotationMode::VelocityDirection);
		}
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
		Speed = WALK_SPEED;
		break;
		case ELSGait::Sprinting:
		Speed = SPRINT_SPEED;
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
	Super::PickupObjects();
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
}

void AMockCharacter::StopSprint()
{
	Super::StopSprint();
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

	if (InventoryComponent->EmptyWeaponInventory())
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

	if (BackPack)
	{
		BackPack->StopSimulatePhysics();
	}

	StopAiming();
	//ReleaseAllWeaponInventory();
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

	if (BackPack)
	{
		BackPack->StartSimulatePhysics();
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
	CurrentWeapon.Get()->SetActorHiddenInGame(false);
	Super::Equipment_Implementation();
}

void AMockCharacter::UnEquipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	IAttackInstigator::Execute_DoFireReleassed(this);

	bool bPutWeaponSuccess = false;
	BackPack->PutWeapon(CurrentWeapon.Get(), bPutWeaponSuccess);
	if (!bPutWeaponSuccess)
	{
		const FName SocketName(CurrentWeapon.Get()->GetWeaponItemInfo().UnEquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		CurrentWeapon.Get()->SetEquip(false);
	}
	CurrentWeapon.Reset();
	ActionInfoPtr = nullptr;
}

bool AMockCharacter::CanPickup_Implementation() const
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
				const bool bWasSameWeapon = Super::WasSameWeaponType(Weapon);
				if (bWasSameWeapon)
				{
					return;
				}

				FWeaponItemInfo WeaponItemInfo = Weapon->GetWeaponItemInfo();
				CreateWeaponInstance(Weapon->GetTemplateClass(), [&](AAbstractWeapon* InWeapon)
				{
					if (InWeapon)
					{
						InWeapon->CopyWeaponItemInfo(&WeaponItemInfo);
					}
				});
				IInteractionItem::Execute_Release(Weapon, this);
				Actor = nullptr;
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
					IInteractionItem::Execute_Release(Item, this);
					Actor = nullptr;
				}
			}
		}
		break;
	}

}

void AMockCharacter::SetALSCameraShake_Implementation(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale)
{
	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(InShakeClass, InScale, ECameraAnimPlaySpace::Type::CameraLocal);
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
		SetActionInfo(WeaponPtr->GetWeaponItemType());
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

FVector AMockCharacter::BulletTraceRelativeLocation() const
{
	//TPSCameraComponent->GetComponentLocation()
	const FVector Position = (PlayerController ? PlayerController->GetCameraRelativeLocation() : FVector::ZeroVector);
	return bAiming ? Position : Super::BulletTraceRelativeLocation();
}

FVector AMockCharacter::BulletTraceForwardLocation() const
{
	//TPSCameraComponent->GetForwardVector()
	const FVector Position = (PlayerController ? PlayerController->GetCameraForwardVector() : FVector::ZeroVector);
	const FVector ForwardLocation = bAiming ? Position : GetActorForwardVector();
	const float TraceDistance = CurrentWeapon.IsValid() ? CurrentWeapon.Get()->GetTraceDistance() : ZERO_VALUE;
	return BulletTraceRelativeLocation() + (ForwardLocation * TraceDistance);
}

void AMockCharacter::OverlapActor(AActor* InActor)
{
	if (OutlinePostProcessComponent)
	{
		OutlinePostProcessComponent->bEnabled = InActor ? 1 : 0;
		OutlinePostProcessComponent->bUnbound = InActor ? 1 : 0;
		OutlinePostProcessComponent->BlendWeight = InActor ? 1.0f : 0.0f;
		OutlinePostProcessComponent->SetVisibility(InActor != nullptr);
	}
	Super::OverlapActor(InActor);
}

void AMockCharacter::CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, WeaponFunc Callback)
{
	if (!InWeaponTemplate)
	{
		if (Callback)
		{
			Callback(nullptr);
		}
		return;
	}

	AAbstractWeapon* const SpawningObject = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<AAbstractWeapon>(this, InWeaponTemplate, GetActorTransform(), this);
	if (!SpawningObject)
	{
		if (Callback)
		{
			Callback(nullptr);
		}
		return;
	}

	bool bPutWeaponSuccess = false;
	BackPack->PutWeapon(SpawningObject, bPutWeaponSuccess);
	if (!bPutWeaponSuccess)
	{
		UE_LOG(LogWevetClient, Error, TEXT("PutError : %s"), *SpawningObject->GetName());
	}
	InventoryComponent->AddWeaponInventory(SpawningObject);
	IInteractionItem::Execute_Take(SpawningObject, this);

	if (Callback)
	{
		Callback(SpawningObject);
	}
}

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

void AMockCharacter::SpawnBackPack()
{
	if (!BackPackTemplate)
	{
		return;
	}

	BackPack = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<ABackPack>(this, BackPackTemplate, GetActorTransform(), this);
	if (!BackPack)
	{
		return;
	}

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	BackPack->FinishSpawning(GetActorTransform());
	BackPack->AttachToComponent(Super::GetMesh(), Rules, BACKPACK_SOCKET);
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

	if (InventoryComponent)
	{
		InventoryComponent->SetOwnerNoSeeMesh(NewOwnerNoSee);
	}

	if (BackPack)
	{
		BackPack->SetOwnerNoSeeMesh(NewOwnerNoSee);
	}
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
	Super::RagdollToWakeUpAction();
	Super::SetReplicateMovement(true);
}
