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

#define CAMERA_TRACE_L FName(TEXT("TP_CameraTrace_L"))
#define CAMERA_TRACE_R FName(TEXT("TP_CameraTrace_R"))

using namespace Wevet;

AMockCharacter::AMockCharacter(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	// override baseCharacter
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	CameraTraceLSocket = CAMERA_TRACE_L;
	CameraTraceRSocket = CAMERA_TRACE_R;

	GetMesh()->ComponentTags.Add(WATER_TAG);

	bEnableRagdoll = false;
	bEnableRecover = true;
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

	// BackPack
	static ConstructorHelpers::FObjectFinder<UClass> FindAsset(ProjectFile::GetBackPackPath());
	BackPackTemplate = FindAsset.Object;

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
}

void AMockCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("ReleaseObjects", IE_Pressed, this, &AMockCharacter::ReleaseObjects);
	PlayerInputComponent->BindAction("PickupObjects", IE_Pressed, this, &AMockCharacter::PickupObjects);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMockCharacter::OnEquipWeapon);
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &AMockCharacter::OnChangeWeapon);
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

	// Toggle Walk Running
	PlayerInputComponent->BindAction("WalkAction", IE_Pressed, this, &AMockCharacter::OnWalkAction);

	// Aiming
	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &AMockCharacter::Aiming);
	PlayerInputComponent->BindAction("AimAction", IE_Released, this, &AMockCharacter::StopAiming);

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

void AMockCharacter::OnCrouch()
{
	Super::OnCrouch();
}

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

void AMockCharacter::OnWalkAction()
{
	Super::OnWalkAction();
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

void AMockCharacter::Aiming()
{
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
	Super::Jump();
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
#pragma endregion


#pragma region Interface
void AMockCharacter::Die_Implementation()
{
	if (!ICombatInstigator::Execute_IsDeath(this))
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
	AAbstractWeapon* const WeaponPtr = InventoryComponent->FindByIndexWeapon(WeaponCurrentIndex);
	if (WeaponPtr == nullptr)
	{
		return;
	}

	// SetSmartPointer
	CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(WeaponPtr);
	SetActionInfo(WeaponPtr->GetWeaponItemType());
	if (ActionInfoPtr && ActionInfoPtr->EquipMontage)
	{
		EquipWeaponTimeOut += PlayAnimMontage(ActionInfoPtr->EquipMontage, MONTAGE_DELAY);
	}
}
#pragma endregion

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

void AMockCharacter::SpawnBackPack()
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
	Super::SetReplicateMovement(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, ELSMovementMode::Ragdoll);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
}

