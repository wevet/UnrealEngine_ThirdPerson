// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockCharacter.h"
#include "Player/MockPlayerController.h"

#include "AnimInstance/PlayerAnimInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/RotationMatrix.h"

#include "Wevet.h"
#include "WevetExtension.h"
#include "Net/UnrealNetwork.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"

#define CAMERA_TRACE_L FName(TEXT("TP_CameraTrace_L"))
#define CAMERA_TRACE_R FName(TEXT("TP_CameraTrace_R"))


AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	// override baseCharacter
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	CameraTraceLSocket = CAMERA_TRACE_L;
	CameraTraceRSocket = CAMERA_TRACE_R;

	GetMesh()->ComponentTags.Add(WATER_TAG);

	bEnableRagdoll = false;
	SetEnableRecover(true);
	WeaponCurrentIndex = 0;

	Tags.Add(PLAYER_TAG);

	// Item Pickup PP
	OutlinePostProcessComponent = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("OutlinePostProcessComponent"));
	OutlinePostProcessComponent->bAutoActivate = 1;
	OutlinePostProcessComponent->bEnabled = 0;
	OutlinePostProcessComponent->bUnbound = 0;
	OutlinePostProcessComponent->BlendWeight = 0.0f;
	OutlinePostProcessComponent->SetVisibility(false);
	OutlinePostProcessComponent->SetupAttachment(GetCapsuleComponent());

	// @TODO Dealth PP
	DeathPostProcessComponent = ObjectInitializer.CreateDefaultSubobject<UPostProcessComponent>(this, TEXT("DeathPostProcessComponent"));
	DeathPostProcessComponent->bAutoActivate = 1;
	DeathPostProcessComponent->bEnabled = 0;
	DeathPostProcessComponent->bUnbound = 0;
	DeathPostProcessComponent->BlendWeight = 0.0f;
	DeathPostProcessComponent->SetVisibility(false);
	DeathPostProcessComponent->SetupAttachment(GetCapsuleComponent());

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetBackPackPath());
		BackPackTemplate = FindAsset.Object;
	}

	// TeamID = 0
	// LookingDirection Initialize
	ALSRotationMode = ELSRotationMode::LookingDirection;
}


void AMockCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


void AMockCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void AMockCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerAnimInstance = Cast<UPlayerAnimInstance>(GetAnimInstance());
	GetOutlinePostProcessComponent()->AddTickPrerequisiteActor(this);
	GetDeathPostProcessComponent()->AddTickPrerequisiteActor(this);
	CreateBackPack();

	CreateWeaponInstance(PrimaryWeapon, [&](AAbstractWeapon* Weapon)
	{
		//
	});

	CreateWeaponInstance(SecondaryWeapon, [&](AAbstractWeapon* Weapon) 
	{
		//
	});
}


void AMockCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<AMockPlayerController>(NewController);
}


void AMockCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// joystick Input
	PlayerInputComponent->BindAxis("LookRight", this, &AMockCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMockCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMockCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMockCharacter::MoveRight);

	// Weapon Events
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &AMockCharacter::OnChangeWeapon);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMockCharacter::OnEquipWeapon);


	// Aiming
	FInputActionBinding AimPressed("AimAction", IE_Pressed);
	AimPressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		ILocomotionSystemPawn::Execute_SetALSAiming(this, true);
	});
	FInputActionBinding AimReleased("AimAction", IE_Released);
	AimReleased.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		ILocomotionSystemPawn::Execute_SetALSAiming(this, false);
	});
	PlayerInputComponent->AddActionBinding(AimPressed);
	PlayerInputComponent->AddActionBinding(AimReleased);


	// Pickup / Release
	FInputActionBinding ReleasePressed("ReleaseObjects", IE_Pressed);
	ReleasePressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::ReleaseObjects();
	});
	FInputActionBinding PickupPressed("PickupObjects", IE_Pressed);
	PickupPressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::PickupObjects();
	});
	PlayerInputComponent->AddActionBinding(ReleasePressed);
	PlayerInputComponent->AddActionBinding(PickupPressed);

	
	// Crouch
	FInputActionBinding CrouchPressed("CrouchAction", IE_Pressed);
	CrouchPressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::OnCrouch();
	});
	PlayerInputComponent->AddActionBinding(CrouchPressed);


	// Fire Press/Released/Reload
	FInputActionBinding FirePressed("Fire", IE_Pressed);
	FirePressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::DoFirePressed_Implementation();
	});
	FInputActionBinding FireReleased("Fire", IE_Released);
	FireReleased.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::DoFireReleassed_Implementation();
	});
	FInputActionBinding ReloadPressed("Reload", IE_Pressed);
	ReloadPressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::DoReload_Implementation();
	});
	PlayerInputComponent->AddActionBinding(FirePressed);
	PlayerInputComponent->AddActionBinding(FireReleased);
	PlayerInputComponent->AddActionBinding(ReloadPressed);


	// MeleeAttack
	FInputActionBinding MeleePressed("MeleeAttack", IE_Pressed);
	MeleePressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::DoMeleeAttack_Implementation();
	});
	PlayerInputComponent->AddActionBinding(MeleePressed);


	// Jump
	FInputActionBinding JumpPressed("JumpAction", IE_Pressed);
	JumpPressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::Jump();
	});
	FInputActionBinding JumpReleased("JumpAction", IE_Released);
	JumpReleased.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::StopJumping();
	});
	PlayerInputComponent->AddActionBinding(JumpPressed);
	PlayerInputComponent->AddActionBinding(JumpReleased);


	// Sprint
	FInputActionBinding SprintPressed("SprintAction", IE_Pressed);
	SprintPressed.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::Sprint();
	});
	FInputActionBinding SprintReleased("SprintAction", IE_Released);
	SprintReleased.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::StopSprint();
	});
	PlayerInputComponent->AddActionBinding(SprintPressed);
	PlayerInputComponent->AddActionBinding(SprintReleased);


	// Walk Running
	FInputActionBinding WalkAction("WalkAction", IE_Pressed);
	WalkAction.ActionDelegate.GetDelegateForManualSet().BindLambda([this]()
	{
		Super::OnWalkAction();
	});
	PlayerInputComponent->AddActionBinding(WalkAction);
}


#pragma region Input
void AMockCharacter::OnChangeWeapon()
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
		WeaponCurrentIndex = 0;
		return;
	}

	const int32 LastIndex = (InventoryComponent->GetWeaponInventory().Num() - 1);
	if (WeaponCurrentIndex >= LastIndex)
	{
		WeaponCurrentIndex = 0;
	}
	else
	{
		++WeaponCurrentIndex;
	}
}

void AMockCharacter::OnEquipWeapon()
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
	Super::MoveForward(Value);
}

void AMockCharacter::MoveRight(float Value)
{
	Super::MoveRight(Value);
}
#pragma endregion


#pragma region Interface
void AMockCharacter::Die_Implementation()
{
	if (!ICombatInstigator::Execute_IsDeath(this))
	{
		CharacterModel->DoTakeDamage(CharacterModel->GetMaxHealth());
		CharacterModel->DoDie();
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

	ILocomotionSystemPawn::Execute_SetALSAiming(this, false);
	StartRagdollAction();
}


void AMockCharacter::Alive_Implementation()
{
	Super::Alive_Implementation();

	if (BackPack)
	{
		BackPack->StartSimulatePhysics();
	}

	VisibleDeathPostProcess(false);
	RagdollToWakeUpAction();
}


void AMockCharacter::Equipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}
	Super::Equipment_Implementation();
}


void AMockCharacter::UnEquipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	ICombatInstigator::Execute_DoFireReleassed(this);

	bool bPutWeaponSuccess = false;
	BackPack->StoreWeapon(CurrentWeapon.Get(), bPutWeaponSuccess);
	if (!bPutWeaponSuccess)
	{
		UE_LOG(LogWevetClient, Error, TEXT("PutError : %s"), *CurrentWeapon.Get()->GetName());
	}
	CurrentWeapon.Reset();
	ActionInfoPtr = nullptr;
}


bool AMockCharacter::CanPickup_Implementation() const
{
	return !ICombatInstigator::Execute_IsDeath(this);
}


void AMockCharacter::Release_Implementation()
{
	const FVector ForwardOffset = GetController() ? GetController()->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (AAbstractWeapon* Weapon = InventoryComponent->GetUnEquipWeaponByIndex(WeaponCurrentIndex))
	{
		InventoryComponent->RemoveWeaponInventory(Weapon);
		ReleaseWeaponToWorld(Transform, Weapon);
		// Refresh Array Index
		WeaponCurrentIndex = 0;
	}
}


void AMockCharacter::OverlapActor_Implementation(AActor* Actor)
{
	if (OutlinePostProcessComponent)
	{
		OutlinePostProcessComponent->bEnabled = Actor ? 1 : 0;
		OutlinePostProcessComponent->bUnbound = Actor ? 1 : 0;
		OutlinePostProcessComponent->BlendWeight = Actor ? 1.0f : 0.0f;
		OutlinePostProcessComponent->SetVisibility(Actor != nullptr);
	}
	Super::OverlapActor_Implementation(Actor);
}


void AMockCharacter::SetALSCameraShake_Implementation(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale)
{
	if (PlayerController)
	{
		// RPC's UFUNCTION unreliable, client
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
	AAbstractWeapon* const WeaponPtr = FindByIndexWeapon();
	if (WeaponPtr == nullptr)
	{
		return;
	}

	CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(WeaponPtr);
	SetActionInfo(WeaponPtr->GetWeaponItemType());

	if (ActionInfoPtr && ActionInfoPtr->EquipMontage)
	{
		EquipWeaponTimeOut += PlayAnimMontage(ActionInfoPtr->EquipMontage, MONTAGE_DELAY);
	}
}
#pragma endregion


AAbstractWeapon* AMockCharacter::FindByIndexWeapon()
{
	return GetInventoryComponent()->FindByIndexWeapon(WeaponCurrentIndex);
}


FVector AMockCharacter::BulletTraceRelativeLocation() const
{
	const FVector Position = (PlayerController ? PlayerController->GetCameraRelativeLocation() : Super::BulletTraceRelativeLocation());
	return bAiming ? Position : Super::BulletTraceRelativeLocation();
}


FVector AMockCharacter::BulletTraceForwardLocation() const
{
	const FTransform MuzzleTransform = CurrentWeapon.IsValid() ? CurrentWeapon.Get()->GetMuzzleTransform() : FTransform::Identity;
	const FRotator MuzzleRotation = FRotator(MuzzleTransform.GetRotation());
	const float TraceDistance = CurrentWeapon.IsValid() ? CurrentWeapon.Get()->GetTraceDistance() : ZERO_VALUE;

	const FVector Position = (PlayerController ? PlayerController->GetCameraForwardVector() : MuzzleRotation.Vector());
	const FVector ForwardLocation = bAiming ? Position : MuzzleRotation.Vector();
	return BulletTraceRelativeLocation() + (ForwardLocation * TraceDistance);
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
	BackPack->StoreWeapon(SpawningObject, bPutWeaponSuccess);
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


void AMockCharacter::CreateBackPack()
{
	if (!BackPackTemplate)
	{
		return;
	}

	BackPack = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<ABackPack>(this, BackPackTemplate, GetActorTransform(), this);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	BackPack->FinishSpawning(GetActorTransform());
	BackPack->AttachToComponent(Super::GetMesh(), Rules, BACKPACK_SOCKET);
}


void AMockCharacter::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	for (UMeshComponent* Component : MeshComponents)
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
	Super::SetReplicateMovement(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, ELSMovementMode::Ragdoll);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
}

