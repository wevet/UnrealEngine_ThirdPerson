// Copyright 2018 wevet works All Rights Reserved.

#include "Character/CharacterBase.h"
#include "AnimInstance/CharacterAnimInstanceBase.h"
#include "AnimInstance/IKAnimInstance.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "LocomotionSystemMacroLibrary.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	TakeDamageTimeOut(0.f),
	MeleeAttackTimeOut(0.f),
	StanTimeOut(30.f)
{
	PrimaryActorTick.bCanEverTick = true;

	JumpMaxHoldTime = 0.f;

	HeadBoneName   = HEAD_BONE;
	ChestBoneName  = CHEST_BONE;
	PelvisBoneName = PELVIS_BONE;

	HeadSocketName   = HEAD_SOCKET;
	ChestSocketName  = CHEST_SOCKET;
	PelvisSocketName = PELVIS_SOCKET;
	GiveDamageType = EGiveDamageType::None;
	RagdollPoseSnapshot = FName(TEXT("RagdollPose"));

	bCrouch  = false;
	bSprint  = false;
	bWasDied = false;
	bEnableRagdoll = true;
	bDebugTrace = false;
	bWasStanning = false;
	bAiming = false;
	bWasMoving = false;
	bWasMovementInput = false;
	bRagdollOnGround = false;

	// Noise Emitter
	PawnNoiseEmitterComponent = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("PawnNoiseEmitterComponent"));
	PawnNoiseEmitterComponent->bAutoActivate = 1;
	PawnNoiseEmitterComponent->bAutoRegister = 1;

	// Combo
	ComboComponent = ObjectInitializer.CreateDefaultSubobject<UComboComponent>(this,  TEXT("ComboComponent"));
	ComboComponent->bAutoActivate = 1;
	ComboComponent->bAutoRegister = 1;

	// ItemPickup
	PickupComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterPickupComponent>(this, TEXT("PickupComponent"));
	PickupComponent->bAutoActivate = 1;
	PickupComponent->bAutoRegister = 1;

	// Inventory
	InventoryComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterInventoryComponent>(this, TEXT("InventoryComponent"));
	InventoryComponent->bAutoActivate = 1;
	InventoryComponent->bAutoRegister = 1;

	// CutScene Voice
	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = 1;
	AudioComponent->bAutoDestroy = 1;
	AudioComponent->SetupAttachment(GetMesh(), HEAD_BONE);

	// ALS Override
	GetCharacterMovement()->BrakingFrictionFactor = ZERO_VALUE;
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = 1;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = 1;
	GetCharacterMovement()->NavAgentProps.bCanFly = 1;

	// SwimParams
	GetCharacterMovement()->Buoyancy = 1.3f;

	GetMesh()->SetCollisionProfileName(FName(TEXT("Pawn")));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->bMultiBodyOverlap = 1;

	//GetMesh()->SetRenderCustomDepth(true);
	//GetMesh()->CustomDepthStencilValue = (int32)ECustomDepthType::Character;

	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.0f);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("Pawn")));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/VFX/Cascade/Gameplay/Blood/P_body_bullet_impact"));
		BloodTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Sound/FootSteps/Footstep_Dry_Cue"));
		FootStepSoundAsset = FindAsset.Object;
	}

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	SetCanBeDamaged(true);
	Tags.Add(CHARACTER_TAG);

	// ALS
	WalkingSpeed = 200.f;
	RunningSpeed = 400.f;
	SprintingSpeed = 800.f;
	CrouchingSpeed = 200.f;
	SwimmingSpeed = 300.f;
	WalkingAcceleration = 800.f;
	RunningAcceleration = 1000.f;
	WalkingDeceleration = 800.f;
	RunningDeceleration = 800.f;
	WalkingGroundFriction = 8.f;
	RunningGroundFriction = 6.f;

	// FallingMantle
	FallingTraceSettings.MaxLedgeHeight = 150.f;
	FallingTraceSettings.MinLedgeHeight = 50.f;
	FallingTraceSettings.ReachDistance = 70.f;
	FallingTraceSettings.ForwardTraceRadius = 30.f;
	FallingTraceSettings.DownwardTraceRadius = 30.f;

	// GroundMantle
	GroundedTraceSettings.MaxLedgeHeight = 250.f;
	GroundedTraceSettings.MinLedgeHeight = 50.f;
	GroundedTraceSettings.ReachDistance = 75.f;
	GroundedTraceSettings.ForwardTraceRadius = 30.f;
	GroundedTraceSettings.DownwardTraceRadius = 30.f;

	// AutomaticMantle
	AutomaticTraceSettings.MaxLedgeHeight = 80.f;
	AutomaticTraceSettings.MinLedgeHeight = 40.f;
	AutomaticTraceSettings.ReachDistance = 50.f;
	AutomaticTraceSettings.ForwardTraceRadius = 30.f;
	AutomaticTraceSettings.DownwardTraceRadius = 30.f;
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FRotator Rotation = GetActorRotation();
	LastVelocityRotation = Rotation;
	LookingRotation = Rotation;
	LastMovementInputRotation = Rotation;
	TargetRotation = Rotation;
	CharacterRotation = Rotation;
}

void ACharacterBase::BeginDestroy()
{
	if (CharacterModel && CharacterModel->IsValidLowLevel())
	{
		CharacterModel->ConditionalBeginDestroy();
		CharacterModel = nullptr;
	}
	Super::BeginDestroy();
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}

	if (GetWorldTimerManager().IsTimerActive(MeleeAttackHundle))
	{
		GetWorldTimerManager().ClearTimer(MeleeAttackHundle);
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}
	if (GetWorldTimerManager().IsTimerActive(StanHundle))
	{
		GetWorldTimerManager().ClearTimer(StanHundle);
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}

	ActionInfoPtr = nullptr;
	IgnoreActors.Reset(0);
	GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &ACharacterBase::HitReceive);
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	IgnoreActors.Add(this);
	GetMesh()->AddTickPrerequisiteActor(this);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACharacterBase::HitReceive);
	ILocomotionSystemPawn::Execute_OnALSViewModeChange(this);
	ILocomotionSystemPawn::Execute_OnALSRotationModeChange(this);
	ILocomotionSystemPawn::Execute_OnALSStanceChange(this);

}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateEssentialVariables();

	if (WasTakeDamagePlaying())
	{
		TakeDamageTimeOut -= DeltaTime;
	}
	if (WasMeleeAttackPlaying())
	{
		MeleeAttackTimeOut -= DeltaTime;
	}
	if (WasEquipWeaponPlaying())
	{
		EquipWeaponTimeOut -= DeltaTime;
	}
}

void ACharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, GetPawnMovementModeChanged(PrevMovementMode, PreviousCustomMode));
}

void ACharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	ILocomotionSystemPawn::Execute_SetALSStance(this, ELSStance::Crouching);
}

void ACharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	ILocomotionSystemPawn::Execute_SetALSStance(this, ELSStance::Standing);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACharacterBase, MovementInput, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ACharacterBase, RagdollLocation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ACharacterBase, CharacterRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ACharacterBase, LookingRotation, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ACharacterBase, TargetRotation, COND_SkipOwner);
}

// AI Perception
FGenericTeamId ACharacterBase::GetGenericTeamId() const
{
	return TeamId;
}

// AI Perception
// ttps://blog.gamedev.tv/ai-sight-perception-to-custom-points/
bool ACharacterBase::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	static const FName AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;

	TArray<USkeletalMeshSocket*> Sockets = GetMesh()->SkeletalMesh->GetActiveSocketList();

	// Check the line of sight for the acquired Socket
	for (int i = 0; i < Sockets.Num(); i++)
	{
		const FVector SocketLocation = GetMesh()->GetSocketLocation(Sockets[i]->SocketName);

		// Trace each Socket of the target Character from the NPC
		const bool bHitResult = GetWorld()->LineTraceSingleByObjectType(
			HitResult, 
			ObserverLocation, 
			SocketLocation, 
			FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)), 
			FCollisionQueryParams(AILineOfSight, true, IgnoreActor));
		NumberOfLoSChecksPerformed++;

		// It is assumed that there is a line of sight between the NPC and the socket (unless there is anything blocking it).
		if (!bHitResult || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
		{
			OutSeenLocation = SocketLocation;
			OutSightStrength = 1;
			//UE_LOG(LogWevetClient, Warning, TEXT("Socket Name: %s"), *Sockets[i]->SocketName.ToString());
			return true;
		}
	}

	// If all sockets are TRUE (obstructed by something), Trace the position of the target Root Component.
	const bool bHit = GetWorld()->LineTraceSingleByObjectType(
		HitResult, 
		ObserverLocation, 
		GetActorLocation(), 
		FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)), 
		FCollisionQueryParams(AILineOfSight, true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if (!bHit || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;
		return true;
	}

	OutSightStrength = 0;
	return false;
}

#pragma region InteractionPawn
void ACharacterBase::Pickup_Implementation(const EItemType InItemType, AActor* Actor)
{
}

bool ACharacterBase::CanPickup_Implementation() const
{
	return false;
}

void ACharacterBase::Release_Implementation()
{
}
#pragma endregion

#pragma region SoundInstigator
void ACharacterBase::ReportNoise_Implementation(USoundBase* Sound, float Volume)
{
	if (!GetWorld())
	{
		return;
	}

	if (Sound)
	{
		//MakeNoise(Volume, this, GetActorLocation());
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::FootStep_Implementation(USoundBase* Sound, float Volume)
{
	USoundBase* const InSound = Sound ? Sound : FootStepSoundAsset;
	if (InSound)
	{
		//MakeNoise(InVolume, this, GetActorLocation());
		const float Speed = GetVelocity().Size();
		const float InVolume = FMath::Clamp<float>((Speed / GetCharacterMovement()->MaxWalkSpeed), MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InSound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location)
{
	if (Sound)
	{
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(Actor, InVolume, Location);
	}
}
#pragma endregion

#pragma region DamageInstigator
float ACharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IDamageInstigator::Execute_IsDeath(this))
	{
		return DEFAULT_VALUE;
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	CharacterModel->TakeDamage((int32)ActualDamage);
	const bool bDeath = CharacterModel->IsEmptyHealth();

	if (bDeath)
	{
		CharacterModel->Die();
		IDamageInstigator::Execute_Die(this);
	}
	IDamageInstigator::Execute_InfrictionDamage(EventInstigator->GetPawn(), this, bDeath);
	return ActualDamage;
}

bool ACharacterBase::CanKillDealDamage_Implementation(const FName BoneName) const
{
	if (BoneName == HeadBoneName) 
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Super::GetMesh();
		auto RefSkeleton = SkeletalMeshComponent->SkeletalMesh->Skeleton->GetReferenceSkeleton();
		if (RefSkeleton.FindBoneIndex(BoneName) != INDEX_NONE)
		{
			return true;
		}
	} 
	return false;
}

float ACharacterBase::MakeDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage) const
{
	const int32 TotalAttack = CharacterModel->GetAttack() + InWeaponDamage;
	const int32 TotalDeffence = DamageModel->GetDefence() + DamageModel->GetWisdom();
	const int32 TotalDamage = (TotalAttack - TotalDeffence);
	float Damage = (float)TotalDamage;
	return FMath::Abs(Damage);
}

bool ACharacterBase::IsDeath_Implementation() const
{
	if (bWasDied || !CharacterModel)
	{
		return true;
	}
	return CharacterModel->IsDie();
}

void ACharacterBase::InfrictionDamage_Implementation(AActor* InfrictionActor, const bool bInfrictionDie)
{
	if (bInfrictionDie)
	{
		if (KillDelegate.IsBound())
		{
			KillDelegate.Broadcast(InfrictionActor);
		}
	}
}

void ACharacterBase::Die_Implementation()
{
	if (bWasDied)
	{
		return;
	}

	bWasDied = true;
	SetReplicateMovement(false);

	TearOff();

	CurrentWeapon.Reset();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ReleaseAllWeaponInventory();
	KillRagdollPhysics();
}

void ACharacterBase::Alive_Implementation()
{
	if (CharacterModel)
	{
		CharacterModel->Alive();
	}
}

void ACharacterBase::Equipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		const FName SocketName(CurrentWeapon.Get()->GetWeaponItemInfo().EquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		CurrentWeapon.Get()->SetEquip(true);
	}
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		IAttackInstigator::Execute_DoFireReleassed(this);
		const FName SocketName(CurrentWeapon.Get()->GetWeaponItemInfo().UnEquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		CurrentWeapon.Get()->SetEquip(false);
	}
	ActionInfoPtr = nullptr;
}

UCharacterModel* ACharacterBase::GetPropertyModel_Implementation() const
{
	return CharacterModel;
}

void ACharacterBase::HitEffectReceive_Implementation(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType)
{
	GiveDamageType = InGiveDamageType;

	switch (GiveDamageType)
	{
		case EGiveDamageType::None:
		case EGiveDamageType::Shoot:
		{
			FTransform Transform;
			Transform.SetIdentity();
			Transform.SetLocation(HitResult.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodTemplate, Transform, true);
			TakeDamageMontage(false);
		}
		break;
		case EGiveDamageType::Melee:
		{
			TakeDamageMontage(true);
			StartRagdollAction();
		}
		break;
	}
}
#pragma endregion

#pragma region AttackInstigator
void ACharacterBase::DoFirePressed_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoFirePressed(CurrentWeapon.Get());
	}
}

void ACharacterBase::DoFireReleassed_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoFireRelease(CurrentWeapon.Get());
	}
}

void ACharacterBase::DoMeleeAttack_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		MeleeAttackMontage();
	}
}

void ACharacterBase::DoReload_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoReload(CurrentWeapon.Get());
	}
}
#pragma endregion

#pragma region ALSInterface
ELSMovementMode ACharacterBase::GetALSMovementMode_Implementation() const
{
	return ALSMovementMode;
}

ELSMovementAction ACharacterBase::GetALSMovementAction_Implementation() const
{
	return ALSMovementAction;
}

ELSGait ACharacterBase::GetALSGait_Implementation() const
{
	return ALSGait;
}

ELSStance ACharacterBase::GetALSStance_Implementation() const
{
	return ALSStance;
}

ELSViewMode ACharacterBase::GetALSViewMode_Implementation() const
{
	return ALSViewMode;
}

ELSRotationMode ACharacterBase::GetALSRotationMode_Implementation() const
{
	return ALSRotationMode;
}

bool ACharacterBase::HasMovementInput_Implementation() const
{
	return bWasMovementInput;
}

bool ACharacterBase::HasAiming_Implementation() const
{
	return bAiming;
}

FTransform ACharacterBase::GetPivotTarget_Implementation() const
{
	return GetActorTransform();
}

FVector ACharacterBase::GetCameraTarget_Implementation() const
{
	return GetMesh()->GetSocketLocation(FName(TEXT("FPS_Socket")));
}

void ACharacterBase::SetALSCharacterRotation_Implementation(const FRotator AddAmount)
{
	//UE_LOG(LogWevetClient, Log, TEXT("Rotation : %s"), *FString(__FUNCTION__));
}

void ACharacterBase::SetALSCameraShake_Implementation(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale)
{
}

void ACharacterBase::SetALSMovementMode_Implementation(const ELSMovementMode NewALSMovementMode)
{
	if (ALSMovementMode == NewALSMovementMode)
	{
		return;
	}
	ALSPrevMovementMode = ALSMovementMode;
	ALSMovementMode = NewALSMovementMode;
	ILocomotionSystemPawn::Execute_OnALSMovementModeChange(this);
}

void ACharacterBase::OnALSMovementModeChange_Implementation()
{
	ILocomotionSystemPawn::Execute_SetALSMovementMode(GetAnimInstance(), ALSMovementMode);
	//ILocomotionSystemPawn::Execute_SetALSMovementMode(GetIKAnimInstance(), ALSMovementMode);
	UpdateCharacterMovementSettings();
}

void ACharacterBase::SetALSMovementAction_Implementation(const ELSMovementAction NewALSMovementAction)
{
	if (ALSMovementAction == NewALSMovementAction)
	{
		return;
	}
	ALSPrevMovementAction = ALSMovementAction;
	ALSMovementAction = NewALSMovementAction;
	ILocomotionSystemPawn::Execute_OnALSMovementActionChange(this);
}

void ACharacterBase::OnALSMovementActionChange_Implementation()
{
	//
}

void ACharacterBase::SetALSGait_Implementation(const ELSGait NewALSGait)
{
	if (ALSGait == NewALSGait)
	{
		return;
	}
	ALSGait = NewALSGait;
}

void ACharacterBase::OnALSGaitChange_Implementation()
{
	ILocomotionSystemPawn::Execute_SetALSGait(GetAnimInstance(), ALSGait);
	UpdateCharacterMovementSettings();
}

void ACharacterBase::SetALSStance_Implementation(const ELSStance NewALSStance)
{
	if (ALSStance == NewALSStance)
	{
		return;
	}
	ALSStance = NewALSStance;
	ILocomotionSystemPawn::Execute_OnALSStanceChange(this);
}

void ACharacterBase::OnALSStanceChange_Implementation()
{
	ILocomotionSystemPawn::Execute_SetALSStance(GetAnimInstance(), ALSStance);
	//ILocomotionSystemPawn::Execute_SetALSStance(GetIKAnimInstance(), ALSStance);
	UpdateCharacterMovementSettings();
}

void ACharacterBase::SetALSRotationMode_Implementation(const ELSRotationMode NewALSRotationMode)
{
	if (ALSRotationMode == NewALSRotationMode)
	{
		return;
	}
	ALSRotationMode = NewALSRotationMode;
}

void ACharacterBase::OnALSRotationModeChange_Implementation()
{
	ILocomotionSystemPawn::Execute_SetALSRotationMode(GetAnimInstance(), ALSRotationMode);

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

void ACharacterBase::SetALSViewMode_Implementation(const ELSViewMode NewALSViewMode)
{
	if (ALSViewMode == NewALSViewMode)
	{
		return;
	}
	ALSViewMode = NewALSViewMode;
	ILocomotionSystemPawn::Execute_OnALSViewModeChange(this);
}

void ACharacterBase::OnALSViewModeChange_Implementation()
{
	ILocomotionSystemPawn::Execute_SetALSViewMode(GetAnimInstance(), ALSViewMode);
	switch (ALSViewMode)
	{
		case ELSViewMode::ThirdPerson:
		{
			//
		}
		break;
		case ELSViewMode::FirstPerson:
		{
			ILocomotionSystemPawn::Execute_SetALSRotationMode(this, ELSRotationMode::LookingDirection);
		}
		break;
	}
}

void ACharacterBase::SetALSAiming_Implementation(const bool NewALSAiming)
{
	if (bAiming == NewALSAiming)
	{
		return;
	}
	bAiming = NewALSAiming;
}

void ACharacterBase::OnALSAimingChange_Implementation()
{
	ILocomotionSystemPawn::Execute_SetALSAiming(GetAnimInstance(), bAiming);
	UpdateCharacterMovementSettings();
}

void ACharacterBase::SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState)
{
	//
}

void ACharacterBase::SetWalkingSpeed_Implementation(const float InWalkingSpeed)
{
	WalkingSpeed = InWalkingSpeed;
	ILocomotionSystemPawn::Execute_SetWalkingSpeed(GetAnimInstance(), WalkingSpeed);
}

void ACharacterBase::SetRunningSpeed_Implementation(const float InRunningSpeed)
{
	RunningSpeed = InRunningSpeed;
	ILocomotionSystemPawn::Execute_SetRunningSpeed(GetAnimInstance(), RunningSpeed);
}

void ACharacterBase::SetSprintingSpeed_Implementation(const float InSprintingSpeed)
{
	SprintingSpeed = InSprintingSpeed;
	ILocomotionSystemPawn::Execute_SetSprintingSpeed(GetAnimInstance(), SprintingSpeed);
}

void ACharacterBase::SetCrouchingSpeed_Implementation(const float InCrouchingSpeed)
{
	CrouchingSpeed = InCrouchingSpeed;
	ILocomotionSystemPawn::Execute_SetCrouchingSpeed(GetAnimInstance(), CrouchingSpeed);
}

void ACharacterBase::SetSwimmingSpeed_Implementation(const float InSwimmingSpeed)
{
	SwimmingSpeed = InSwimmingSpeed;
	ILocomotionSystemPawn::Execute_SetSwimmingSpeed(GetAnimInstance(), SwimmingSpeed);
}

float ACharacterBase::GetWalkingSpeed_Implementation() const
{
	return WalkingSpeed; 
}

float ACharacterBase::GetRunningSpeed_Implementation() const 
{
	return RunningSpeed; 
}

float ACharacterBase::GetSprintingSpeed_Implementation() const 
{
	return SprintingSpeed; 
}

float ACharacterBase::GetCrouchingSpeed_Implementation() const 
{
	return CrouchingSpeed; 
}

float ACharacterBase::GetSwimmingSpeed_Implementation() const 
{
	return SwimmingSpeed; 
}

void ACharacterBase::SetGetup_Implementation(const bool InFaceDown)
{

}

void ACharacterBase::SetRF_Implementation(const bool InRF)
{
}

void ACharacterBase::PoseSnapShot_Implementation(const FName InPoseName)
{
}

FCameraFOVParam ACharacterBase::GetCameraFOVParam_Implementation() const
{
	return CameraFOVParam;
}

FCameraTraceParam ACharacterBase::GetCameraTraceParam_Implementation() const
{
	FCameraTraceParam Result;
	return Result;
}

bool ACharacterBase::HasDebugTrace_Implementation() const
{
	return bDebugTrace;
}
#pragma endregion

#pragma region Input
void ACharacterBase::Jump()
{
	if (bCrouch)
	{
		OnCrouch();
	}
	Super::Jump();
}

void ACharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ACharacterBase::Sprint()
{
	if (!bSprint)
	{
		bSprint = true;
	}

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

void ACharacterBase::StopSprint()
{
	if (bSprint)
	{
		bSprint = false;
	}
}

void ACharacterBase::OnCrouch()
{
	bCrouch = !bCrouch;
	OnCrouchUpdate();
}

void ACharacterBase::CrouchUpdate(const bool InCrouch)
{
	if (bCrouch != InCrouch)
	{
		bCrouch = InCrouch;
		OnCrouchUpdate();
	}
}

void ACharacterBase::OnCrouchUpdate()
{
	Super::bIsCrouched = bCrouch ? 0 : 1;
	if (bCrouch)
	{
		if (Super::CanCrouch())
		{
			Super::Crouch();
		}
	}
	else
	{
		Super::UnCrouch();
	}
}

void ACharacterBase::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Dir, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Dir, Value);
	}
}

void ACharacterBase::PickupObjects()
{
	AActor* Actor = GetPickupComponent()->GetPickupActor();
	if (Actor == nullptr)
	{
		return;
	}

	IInteractionItem* Interface = Cast<IInteractionItem>(Actor);
	if (Interface == nullptr)
	{
		return;
	}
	Pickup_Implementation(IInteractionItem::Execute_GetItemType(Actor), Actor);
}

void ACharacterBase::ReleaseObjects()
{
}
#pragma endregion

#pragma region Weapon
AAbstractWeapon* ACharacterBase::FindByWeapon(const EWeaponItemType WeaponItemType) const
{
	if (InventoryComponent->EmptyWeaponInventory())
	{
		return nullptr;
	}
	for (AAbstractWeapon* Weapon : InventoryComponent->GetWeaponInventory())
	{
		if (Weapon && Weapon->WasSameWeaponType(WeaponItemType))
		{
			return Weapon;
		}
	}
	return nullptr;
}

EWeaponItemType ACharacterBase::GetCurrentWeaponType() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetWeaponItemInfo().WeaponItemType;
	}
	return EWeaponItemType::None;
}

bool ACharacterBase::HasEquipWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->WasEquip();
	}
	return false;
}

const bool ACharacterBase::WasSameWeaponType(AAbstractWeapon* const Weapon)
{
	if (!Weapon)
	{
		return false;
	}

	if (AAbstractWeapon* const InWeapon = FindByWeapon(Weapon->GetWeaponItemInfo().WeaponItemType))
	{
		return InWeapon->WasSameWeaponType(Weapon->GetWeaponItemInfo().WeaponItemType);
	}
	return false;
}

void ACharacterBase::CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, WeaponFunc Callback)
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

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	SpawningObject->FinishSpawning(GetActorTransform());
	SpawningObject->AttachToComponent(Super::GetMesh(), Rules, SpawningObject->GetWeaponItemInfo().UnEquipSocketName);
	InventoryComponent->AddWeaponInventory(SpawningObject);
	IInteractionItem::Execute_Take(SpawningObject, this);

	if (Callback)
	{
		Callback(SpawningObject);
	}
}

void ACharacterBase::ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon)
{
	if (!Weapon)
	{
		return;
	}

	FWeaponItemInfo WeaponItemInfo = Weapon->GetWeaponItemInfo();
	IInteractionItem::Execute_Release(Weapon, nullptr);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;
	AAbstractWeapon* const WeaponPtr = GetWorld()->SpawnActor<AAbstractWeapon>(Weapon->GetClass(), Transform, SpawnParams);
	if (WeaponPtr)
	{
#if WITH_EDITOR
		WeaponPtr->SetFolderPath("/Weapon");
#endif

		WeaponPtr->CopyWeaponItemInfo(&WeaponItemInfo);
		WeaponPtr->SpawnToWorld();
	}
	Weapon = nullptr;
}

void ACharacterBase::MeleeAttack(const bool InEnable)
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoMeleeAttack(CurrentWeapon.Get(), InEnable);
	}
}

void ACharacterBase::ReleaseAllWeaponInventory()
{
	if (InventoryComponent->EmptyWeaponInventory())
	{
		return;
	}

	TArray<FVector> SpawnPoints;
	const FVector RelativePosition = GetMesh()->GetComponentLocation();
	const int32 WeaponNum = InventoryComponent->GetWeaponInventory().Num();
	UWevetBlueprintFunctionLibrary::CircleSpawnPoints(WeaponNum, DEFAULT_FORWARD_VECTOR, RelativePosition, SpawnPoints);
	for (int Index = 0; Index < WeaponNum; ++Index)
	{
		AAbstractWeapon* Weapon = InventoryComponent->GetWeaponInventory()[Index];
		if (!Weapon)
		{
			continue;
		}
		const FTransform Transform = UKismetMathLibrary::MakeTransform(SpawnPoints[Index], GetActorRotation(), FVector::OneVector);
		ReleaseWeaponToWorld(Transform, Weapon);
	}
	InventoryComponent->ClearWeaponInventory();
}
#pragma endregion

#pragma region Montages
void ACharacterBase::EquipmentActionMontage()
{
	if (InventoryComponent->EmptyWeaponInventory())
	{
		UE_LOG(LogWevetClient, Error, TEXT("Enpty WeaponInventory : %s"), *FString(__FUNCTION__));
		return;
	}

	if (WasEquipWeaponPlaying())
	{
		// Already MontagePlaying
		return;
	}

	if (CurrentWeapon.IsValid())
	{
		SetActionInfo(CurrentWeapon.Get()->GetWeaponItemInfo().WeaponItemType);
		if (ActionInfoPtr && ActionInfoPtr->EquipMontage)
		{
			if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->EquipMontage))
			{
				EquipWeaponTimeOut += PlayAnimMontage(ActionInfoPtr->EquipMontage, MONTAGE_DELAY);
			}
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr EquipMontage : %s"), *FString(__FUNCTION__));
		}
	}
}

void ACharacterBase::UnEquipmentActionMontage()
{
	if (!HasEquipWeapon())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
		return;
	}

	if (ActionInfoPtr && ActionInfoPtr->UnEquipMontage)
	{
		if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->UnEquipMontage))
		{
			PlayAnimMontage(ActionInfoPtr->UnEquipMontage, MONTAGE_DELAY);
		}
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr UnEquipMontage : %s"), *FString(__FUNCTION__));
	}
}

void ACharacterBase::FireActionMontage()
{
	if (!CurrentWeapon.IsValid())
	{
		UE_LOG(LogWevetClient, Error, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
		return;
	}

	if (ActionInfoPtr && ActionInfoPtr->FireMontage)
	{
		PlayAnimMontage(ActionInfoPtr->FireMontage);
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr FireActionMontage : %s"), *FString(__FUNCTION__));
	}
}

void ACharacterBase::ReloadActionMontage(float& OutReloadDuration)
{
	if (!CurrentWeapon.IsValid())
	{
		UE_LOG(LogWevetClient, Error, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
		return;
	}

	if (ActionInfoPtr && ActionInfoPtr->ReloadMontage)
	{
		if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->ReloadMontage))
		{
			OutReloadDuration += PlayAnimMontage(ActionInfoPtr->ReloadMontage);
		}
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr ReloadActionMontage : %s"), *FString(__FUNCTION__));
	}
}

void ACharacterBase::TakeDamageMontage(const bool InForcePlaying)
{
	if (!InForcePlaying)
	{
		if (WasTakeDamagePlaying() || WasMeleeAttackPlaying())
		{
			return;
		}
	}

	check(GetAnimInstance());
	const FName NodeName = (HasMoving() || HasCrouch()) ? UPPER_BODY : FULL_BODY;
	GetAnimInstance()->TakeDefaultDamageAnimation(ActionInfoPtr, NodeName);
}

void ACharacterBase::MeleeAttackMontage()
{
	if (WasMeleeAttackPlaying())
	{
		return;
	}

	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (ActionInfoPtr && ActionInfoPtr->MeleeAttackMontage)
	{
		if (GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->MeleeAttackMontage))
		{
			return;
		}

		MeleeAttackTimeOut = PlayAnimMontage(ActionInfoPtr->MeleeAttackMontage);
		GetCharacterMovement()->DisableMovement();
		//
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([&]
		{
			MeleeAttackTimeOut = ZERO_VALUE;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		});
		GetWorld()->GetTimerManager().SetTimer(MeleeAttackHundle, TimerCallback, MeleeAttackTimeOut, false);
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr MeleeAttackMontage : %s"), *FString(__FUNCTION__));
	}
}
#pragma endregion

#pragma region Utils
FVector ACharacterBase::BulletTraceRelativeLocation() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetMuzzleTransform().GetLocation();
	}
	return GetActorLocation();
}

FVector ACharacterBase::BulletTraceForwardLocation() const
{
	return GetActorForwardVector();
}

FVector ACharacterBase::GetHeadSocketLocation() const
{
	FVector Position = GetActorLocation();
	if (GetMesh())
	{
		// FindBone
		if (GetMesh()->GetBoneIndex(HeadSocketName) != INDEX_NONE)
		{
			FVector BoneLocation = GetMesh()->GetBoneLocation(HeadSocketName);
			Position.Z = BoneLocation.Z;
		}
		else
		{
			// Not Found Try FindSocket
			FVector SocketLocation = GetMesh()->GetSocketLocation(HeadSocketName);
			if (SocketLocation != FVector::ZeroVector)
			{
				Position.Z = SocketLocation.Z;
			}
		}
	}
	return Position;
}

FVector ACharacterBase::GetChestSocketLocation() const
{
	FVector Position = GetActorLocation();
	if (GetMesh())
	{
		// FindBone
		if (GetMesh()->GetBoneIndex(ChestSocketName) != INDEX_NONE)
		{
			FVector BoneLocation = GetMesh()->GetBoneLocation(ChestSocketName);
			Position.Z = BoneLocation.Z;
		}
		else
		{
			// Not Found Try FindSocket
			FVector SocketLocation = GetMesh()->GetSocketLocation(ChestSocketName);
			if (SocketLocation != FVector::ZeroVector)
			{
				Position.Z = SocketLocation.Z;
			}
		}
	}
	return Position;
}

UCharacterAnimInstanceBase* ACharacterBase::GetAnimInstance() const
{
	return Cast<UCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());
}

UIKAnimInstance* ACharacterBase::GetIKAnimInstance() const
{
	return Cast<UIKAnimInstance>(GetMesh()->GetPostProcessInstance());
}

void ACharacterBase::SetActionInfo(const EWeaponItemType InWeaponItemType)
{
	if (GetAnimInstance())
	{
		ActionInfoPtr = GetAnimInstance()->GetActionInfo(InWeaponItemType);
	}
}
#pragma endregion

#pragma region Ragdoll
void ACharacterBase::KillRagdollPhysics()
{
	if (!bEnableRagdoll)
	{
		return;
	}
	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SkelMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	SkelMesh->SetAllBodiesSimulatePhysics(true);
	SkelMesh->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	SkelMesh->WakeAllRigidBodies();
	SkelMesh->bBlendPhysics = 1;
	SkelMesh->bIgnoreRadialForce = 1;
	SkelMesh->bIgnoreRadialImpulse = 1;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACharacterBase::StartRagdollAction()
{
	if (WasStanning())
	{
		// Already Stan
		return;
	}

	UnEquipment_Implementation();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, ELSMovementMode::Ragdoll);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		RagdollToWakeUpAction();
	});
	GetWorld()->GetTimerManager().SetTimer(StanHundle, TimerCallback, StanTimeOut, false);
}

void ACharacterBase::RagdollToWakeUpAction()
{
	GetCharacterMovement()->SetMovementMode(bRagdollOnGround ? EMovementMode::MOVE_Walking : EMovementMode::MOVE_Falling);
	GetCharacterMovement()->Velocity = RagdollVelocity;
	ILocomotionSystemPawn::Execute_PoseSnapShot(GetAnimInstance(), RagdollPoseSnapshot);

	if (bRagdollOnGround)
	{
		const FRotator Rotation = GetMesh()->GetSocketRotation(PelvisBoneName);
		ILocomotionSystemPawn::Execute_SetGetup(GetAnimInstance(), (Rotation.Roll > 0.0f));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}
#pragma endregion

#pragma region HitEvent
void ACharacterBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this)
		return;

	if (ABulletBase * Bullet = Cast<ABulletBase>(Hit.GetActor()))
	{
		IDamageInstigator::Execute_HitEffectReceive(this, Hit, EGiveDamageType::Shoot);
	}
}
#pragma endregion

void ACharacterBase::DoWhileRagdoll(FRotator& OutActorRotation, FVector& OutActorLocation)
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

void ACharacterBase::CalculateActorTransformRagdoll(const FRotator InRagdollRotation, const FVector InRagdollLocation, FRotator& OutActorRotation, FVector& OutActorLocation)
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
		GetDrawDebugTrace(),
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

void ACharacterBase::CalculateEssentialVariables()
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

void ACharacterBase::ManageCharacterRotation()
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

void ACharacterBase::DoCharacterFalling()
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

void ACharacterBase::DoCharacterGrounded()
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

void ACharacterBase::DoWhileGrounded()
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

void ACharacterBase::SetForwardOrRightVector(FVector& OutForwardVector, FVector& OutRightVector)
{
	FRotator Rotation = FRotator::ZeroRotator;
	auto PawnRotation = GetControlRotation();
	Rotation.Yaw = PawnRotation.Yaw;
	OutForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);
	OutRightVector = UKismetMathLibrary::GetRightVector(Rotation);
}

void ACharacterBase::ConvertALSMovementMode()
{
	switch (ALSMovementMode)
	{
		case ELSMovementMode::None:
		case ELSMovementMode::Grounded:
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		break;
		case ELSMovementMode::Swimming:
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Swimming);
		break;
		case ELSMovementMode::Falling:
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		break;
	}
}

const float ACharacterBase::CalculateRotationRate(const float SlowSpeed, const float SlowSpeedRate, const float FastSpeed, const float FastSpeedRate)
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

const FRotator ACharacterBase::LookingDirectionWithOffset(const float OffsetInterpSpeed, const float NEAngle, const float NWAngle, const float SEAngle, const float SWAngle, const float Buffer)
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

ELSMovementMode ACharacterBase::GetPawnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PrevCustomMode) const
{
	switch (GetCharacterMovement()->MovementMode)
	{
		case EMovementMode::MOVE_None:
		case EMovementMode::MOVE_Walking:
		case EMovementMode::MOVE_NavWalking:
		case EMovementMode::MOVE_Custom:
		return ELSMovementMode::Grounded;
		
		case EMovementMode::MOVE_Falling:
		case EMovementMode::MOVE_Flying:
		return ELSMovementMode::Falling;

		case EMovementMode::MOVE_Swimming:
		return ELSMovementMode::Swimming;
	}
	return ELSMovementMode::None;
}

float ACharacterBase::ChooseMaxWalkSpeed() const
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

bool ACharacterBase::CanSprint() const
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

void ACharacterBase::AddCharacterRotation(const FRotator AddAmount)
{
	// Node to InvertRotator
	auto RotateAmount = UKismetMathLibrary::NegateRotator(AddAmount);
	TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, RotateAmount);
	auto RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;
	CharacterRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, RotateAmount);
	SetActorRotation(CharacterRotation);
}

void ACharacterBase::UpdateCharacterMovementSettings()
{
	check(GetCharacterMovement());
	GetCharacterMovement()->MaxWalkSpeed = ChooseMaxWalkSpeed();
	GetCharacterMovement()->MaxWalkSpeedCrouched = ChooseMaxWalkSpeed();
	GetCharacterMovement()->MaxAcceleration = ChooseMaxAcceleration();
	GetCharacterMovement()->BrakingDecelerationWalking = ChooseBrakingDeceleration();
	GetCharacterMovement()->GroundFriction = ChooseGroundFriction();
}

void ACharacterBase::ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed)
{
	TargetRotation = InTargetRotation;
	const FRotator RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;
	CharacterRotation = bInterpRotation ? UKismetMathLibrary::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed) : TargetRotation;
	SetActorRotation(CharacterRotation);
}

void ACharacterBase::LimitRotation(const float AimYawLimit, const float InterpSpeed)
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

bool ACharacterBase::CardinalDirectionAngles(const float Value, const float Min, const float Max, const float Buffer, const ELSCardinalDirection InCardinalDirection) const
{
	const bool A = UKismetMathLibrary::InRange_FloatFloat(Value, (Min + Buffer), (Max - Buffer));
	const bool B = UKismetMathLibrary::InRange_FloatFloat(Value, (Min - Buffer), (Max + Buffer));
	return (CardinalDirection == InCardinalDirection) ? B : A;
}

void ACharacterBase::CustomAcceleration()
{
	const auto Velocity = FMath::Abs(VelocityDifference);
	const float RangeA = 45.f;
	const float RangeB = 130.f;

	auto MaxAccelerationValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.2f);
	GetCharacterMovement()->MaxAcceleration = RunningAcceleration * MaxAccelerationValue;
	auto GroundFrictionValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.4f);
	GetCharacterMovement()->GroundFriction = RunningGroundFriction * GroundFrictionValue;
}

#pragma region MantleUtils
FVector ACharacterBase::GetCapsuleBaseLocation(const float ZOffset) const
{
	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float Value = HalfHeight + ZOffset;
	const FVector UpVector = GetCapsuleComponent()->GetUpVector() * Value;
	const FVector WorldLocation = GetCapsuleComponent()->GetComponentLocation();
	return WorldLocation - UpVector;
}

FVector ACharacterBase::GetCapsuleLocationFromBase(const FVector BaseLocation, const float ZOffset) const
{
	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float Value = HalfHeight + ZOffset;
	FVector Position = FVector::ZeroVector;
	Position.Z = Value;
	Position += BaseLocation;

	return Position;
}

void ACharacterBase::SetActorLocationAndRotation(const FVector NewLocation, const FRotator NewRotation, const bool bWasSweep, const bool bWasTeleport)
{
	TargetRotation = NewRotation;
	Super::SetActorLocationAndRotation(NewLocation, NewRotation, bWasSweep /*nullptr, ETeleportType::None*/);
}

const FVector ACharacterBase::GetPlayerMovementInput()
{
	FVector ForwardVector;
	FVector RightVector;
	SetForwardOrRightVector(ForwardVector, RightVector);
	auto Result = (ForwardVector * ForwardAxisValue) + (RightVector * RightAxisValue);
	const float Tolerance = 0.0001f;
	return UKismetMathLibrary::Normal(Result, Tolerance);
}

bool ACharacterBase::CapsuleHasRoomCheck(const FVector TargetLocation, const float HeightOffset, const float RadiusOffset) const
{
	const float InvRadiusOffset = RadiusOffset * -1.f;
	const float TraceRadius = (GetCapsuleComponent()->GetScaledCapsuleRadius() + RadiusOffset);
	const float Offset = GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere() + InvRadiusOffset + HeightOffset;
	const FName ProfileName(TEXT("ALS_Character"));

	const FVector StartLocation = TargetLocation + FVector(0.f, 0.f, Offset);
	const FVector EndLocation = TargetLocation - FVector(0.f, 0.f, Offset);

	FHitResult HitData(ForceInit);
	UKismetSystemLibrary::SphereTraceSingleByProfile(
		GetWorld(),
		StartLocation,
		EndLocation,
		TraceRadius,
		ProfileName,
		false,
		IgnoreActors,
		GetDrawDebugTrace(),
		HitData,
		true);

	return !(HitData.bBlockingHit || HitData.bStartPenetrating);
}
#pragma endregion

#pragma region MantleSystem
const bool ACharacterBase::MantleCheck(const FMantleTraceSettings InTraceSetting)
{
	FVector InitialTrace_ImpactPoint = FVector::ZeroVector;
	FVector InitialTrace_Normal = FVector::ZeroVector;
	FVector DownTraceLocation = FVector::ZeroVector;
	UPrimitiveComponent* HitComponent = nullptr;
	FTransform TargetTransform = FTransform::Identity;
	float MantleHeight = 0.0f;
	bool OutHitResult = false;


	TraceForwardToFindWall(InTraceSetting, InitialTrace_ImpactPoint, InitialTrace_Normal, OutHitResult);
	if (!OutHitResult)
	{
		//UE_LOG(LogWevetClient, Error, TEXT("Fail SphereTraceByMantleCheck"));
		return OutHitResult;
	}

	SphereTraceByMantleCheck(InTraceSetting, InitialTrace_ImpactPoint, InitialTrace_Normal, OutHitResult, DownTraceLocation, HitComponent);
	if (!OutHitResult)
	{
		//UE_LOG(LogWevetClient, Error, TEXT("Fail SphereTraceByMantleCheck"));
		return OutHitResult;
	}

	ConvertMantleHeight(DownTraceLocation, InitialTrace_Normal, OutHitResult, TargetTransform, MantleHeight);
	if (!OutHitResult)
	{
		//UE_LOG(LogWevetClient, Error, TEXT("Fail ConvertMantleHeight"));
		return OutHitResult;
	}

	FLSComponentAndTransform WS;
	WS.Component = HitComponent;
	WS.Transform = TargetTransform;
	BP_MantleStart(MantleHeight, WS, GetMantleType(MantleHeight));

	return OutHitResult;
}

void ACharacterBase::MantleEnd()
{
	if (IDamageInstigator::Execute_IsDeath(this))
	{
		return;
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

// Step 1: Trace forward to find a wall / object the character cannot walk on.
void ACharacterBase::TraceForwardToFindWall(const FMantleTraceSettings InTraceSetting, FVector& OutInitialTraceImpactPoint, FVector& OutInitialTraceNormal, bool& OutHitResult)
{
	const FVector InputValue = GetPlayerMovementInput();
	const FVector BaseLocation = GetCapsuleBaseLocation(2.0f);

	const float Radius = InTraceSetting.ForwardTraceRadius;

	const float AddLedgeHeigth = (InTraceSetting.MaxLedgeHeight + InTraceSetting.MinLedgeHeight) / 2.f;
	const FVector StartOffset = FVector(0.0f, 0.0f, AddLedgeHeigth);

	FVector StartLocation = BaseLocation + (InputValue * -30.f);
	StartLocation += StartOffset;

	FVector EndLocation = InputValue * InTraceSetting.ReachDistance;
	EndLocation += StartLocation;

	float HalfHeight = (InTraceSetting.MaxLedgeHeight - InTraceSetting.MinLedgeHeight) / 2.f;
	HalfHeight += 1.0f;

	// @NOTE
	// Access For LedgeTrace 
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);
	FHitResult HitData(ForceInit);

	UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), 
		StartLocation, 
		EndLocation, 
		Radius, 
		HalfHeight,
		TraceType,
		false, 
		IgnoreActors, 
		GetDrawDebugTrace(),
		HitData,
		true, 
		FLinearColor::Black, 
		FLinearColor::Blue);

	const bool bWalkableHit = GetCharacterMovement()->IsWalkable(HitData);
	const bool bBlockingHit = HitData.bBlockingHit;
	const bool bInitialOverlap = HitData.bStartPenetrating;

	OutHitResult = (!bWalkableHit && bBlockingHit && !bInitialOverlap);

	if (OutHitResult)
	{
		OutInitialTraceImpactPoint = HitData.ImpactPoint;
		OutInitialTraceNormal = HitData.ImpactNormal;
	}
}

// step2 Trace downward from the first trace's Impact Point and determine if the hit location is walkable.
void ACharacterBase::SphereTraceByMantleCheck(const FMantleTraceSettings TraceSetting, const FVector InitialTraceImpactPoint, const FVector InitialTraceNormal, bool& OutHitResult, FVector& OutDownTraceLocation, UPrimitiveComponent*& OutPrimitiveComponent)
{
	const FVector CapsuleLocation = GetCapsuleBaseLocation(2.0f);
	FVector Normal = InitialTraceNormal * -15.f;
	FVector Value = InitialTraceImpactPoint;
	Value.Z = CapsuleLocation.Z;

	float StartLocationAtZ = TraceSetting.MaxLedgeHeight;
	StartLocationAtZ += TraceSetting.DownwardTraceRadius;
	StartLocationAtZ += 1.0f;
	FVector PreStartLocation(FVector::ZeroVector);
	PreStartLocation.Z = StartLocationAtZ;
	const FVector StartLocation = (Value + Normal + PreStartLocation);
	const FVector EndLocation = (Value + Normal);
	const float Radius = TraceSetting.DownwardTraceRadius;
	
	FHitResult HitData(ForceInit);

	// @NOTE
	// Access For LedgeTrace 
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);

	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(), 
		StartLocation, 
		EndLocation, 
		Radius, 
		TraceType, 
		false, 
		IgnoreActors,
		GetDrawDebugTrace(), 
		HitData, 
		true, 
		FLinearColor::Yellow, 
		FLinearColor::Red);

	const bool bWalkableHit = GetCharacterMovement()->IsWalkable(HitData);
	const bool bResult = HitData.bBlockingHit;

	OutHitResult = (bWalkableHit && bResult);
	OutDownTraceLocation = FVector(HitData.Location.X, HitData.Location.Y, HitData.ImpactPoint.Z);
	OutPrimitiveComponent = HitData.Component.Get();
}

// step3 Check if the capsule has room to stand at the downward trace's location. 
// If so, set that location as the Target Transform and calculate the mantle height.
void ACharacterBase::ConvertMantleHeight(const FVector DownTraceLocation, const FVector InitialTraceNormal, bool& OutRoomCheck, FTransform& OutTargetTransform, float& OutMantleHeight)
{
	const FVector RelativeLocation = GetCapsuleLocationFromBase(DownTraceLocation, 18.0f);
	const FVector Offset = FVector(-1.0f, -1.0f, 0.0f);
	const FRotator RelativeRotation = UKismetMathLibrary::Conv_VectorToRotator(InitialTraceNormal * Offset);

	OutRoomCheck = CapsuleHasRoomCheck(RelativeLocation, 0.0f, 0.0f);
	OutTargetTransform = FTransform::Identity;
	OutTargetTransform.SetLocation(RelativeLocation);
	OutTargetTransform.SetRotation(RelativeRotation.Quaternion());

	const FVector Diff = (RelativeLocation - GetActorLocation());
	OutMantleHeight = Diff.Z;
}

// step4 Determine the Mantle Type by checking the movement mode and Mantle Height.
EMantleType ACharacterBase::GetMantleType(const float InMantleHeight) const
{
	const float LowBorder = 125.f;
	EMantleType Current = EMantleType::HighMantle;
	switch (ALSMovementMode)
	{
		case ELSMovementMode::Falling:
		Current = EMantleType::FallingCatch;
		break;
		case ELSMovementMode::None:
		case ELSMovementMode::Grounded:
		case ELSMovementMode::Ragdoll:
		case ELSMovementMode::Swimming:
		case ELSMovementMode::Mantling:
		if (InMantleHeight < LowBorder)
		{
			Current = EMantleType::LowMantle;
		}
		break;
	}
	return Current;
}
#pragma endregion

#pragma region MantleUpdate
void ACharacterBase::MantleUpdate(const float BlendIn, const float InPlaybackPosition)
{
	float PositionAlpha, XYCorrectionAlpha, ZCorrectionAlpha = ZERO_VALUE;
	SetMantleUpdateAlpha(InPlaybackPosition, PositionAlpha, XYCorrectionAlpha, ZCorrectionAlpha);
	const FTransform LerpedTarget = MakeMantleLerpedTarget(BlendIn, PositionAlpha, XYCorrectionAlpha, ZCorrectionAlpha);

	SetActorLocationAndRotation(LerpedTarget.GetLocation(), FRotator(FQuat(LerpedTarget.GetRotation())), false, false);
}

// Step 1: Continually update the mantle target from the stored local transform to follow along with moving objects.
void ACharacterBase::SetMantleTarget()
{
	auto Component = ULocomotionSystemMacroLibrary::ComponentLocalToWorld(MantleLedgeLS);
	MantleTarget = Component.Transform;
}

// Step 2: Update the Position and Correction Alphas using the Position/Correction curve set for each Mantle.
void ACharacterBase::SetMantleUpdateAlpha(const float InPlaybackPosition, float& OutPositionAlpha, float& OutXYCorrectionAlpha, float& OutZCorrectionAlpha)
{
	auto CurveVector = MantleParams.Position->GetVectorValue(MantleParams.StartingPosition + InPlaybackPosition);
	OutPositionAlpha = CurveVector.X;
	OutXYCorrectionAlpha = CurveVector.Y;
	OutZCorrectionAlpha = CurveVector.Z;
}

// Blend into the animated horizontal and rotation offset using the Y value of the Position/Correction Curve.
FTransform ACharacterBase::MakeXYCollectionAlphaTransform(const float InXYCollectionAlpha) const
{
	FTransform Result = FTransform::Identity;
	auto RotA = MantleAnimatedStartOffset.GetRotation();
	auto PosA = MantleAnimatedStartOffset.GetLocation();
	auto PosB = MantleActualStartOffset.GetLocation();
	PosA.Z = PosB.Z;
	Result.SetRotation(RotA);
	Result.SetLocation(PosA);
	return UKismetMathLibrary::TLerp(MantleActualStartOffset, Result, InXYCollectionAlpha);
}

// Blend into the animated vertical offset using the Z value of the Position/Correction Curve.
FTransform ACharacterBase::MakeZCollectionAlphaTransform(const float InZCollectionAlpha) const
{
	FTransform Result = FTransform::Identity;
	auto RotA = MantleActualStartOffset.GetRotation();
	auto PosA = MantleActualStartOffset.GetLocation();
	auto PosB = MantleAnimatedStartOffset.GetLocation();
	PosA.Z = PosB.Z;
	Result.SetRotation(RotA);
	Result.SetLocation(PosA);
	return UKismetMathLibrary::TLerp(MantleActualStartOffset, Result, InZCollectionAlpha);
}

// Blend from the currently blending transforms into the final mantle target using the X value of the Position/Correction Curve.
FTransform ACharacterBase::MakeMantleTransform(const float InPositionAlpha, const float InXYCollectionAlpha, const float InZCollectionAlpha) const
{
	const FTransform A = MakeXYCollectionAlphaTransform(InXYCollectionAlpha);
	const FTransform B = MakeZCollectionAlphaTransform(InZCollectionAlpha);

	FTransform BlendTransform = FTransform::Identity;
	auto RotA = A.GetRotation();
	auto PosA = A.GetLocation();
	auto PosB = B.GetLocation();
	PosA.Z = PosB.Z;
	BlendTransform.SetRotation(RotA);
	BlendTransform.SetLocation(PosA);

	FTransform Result = ULocomotionSystemMacroLibrary::TransformPlus(MantleTarget, BlendTransform);
	return UKismetMathLibrary::TLerp(Result, MantleTarget, InPositionAlpha);
}

// Step 3: Lerp multiple transforms together for independent control over the horizontal and vertical blend to the animated start position, 
// as well as the target position.
FTransform ACharacterBase::MakeMantleLerpedTarget(const float BlendIn, const float InPositionAlpha, const float InXYCollectionAlpha, const float InZCollectionAlpha) const
{
	const FTransform Result = ULocomotionSystemMacroLibrary::TransformPlus(MantleTarget, MantleActualStartOffset);
	const FTransform MantleTransform = MakeMantleTransform(InPositionAlpha, InXYCollectionAlpha, InZCollectionAlpha);

	return UKismetMathLibrary::TLerp(Result, MantleTransform, BlendIn);
}
#pragma endregion

#pragma region MantleStart
void ACharacterBase::MantleStart(const float InMantleHeight, const FLSComponentAndTransform MantleLedgeWorldSpace, const EMantleType InMantleType, UTimelineComponent* const InMantleTimeline)
{
	check(GetAnimInstance());
	const FMantleAsset MantleAsset = GetAnimInstance()->GetMantleAsset(InMantleType);
	SetMantleParams(MantleAsset, InMantleHeight);
	SetMantleLedgeLocalSpace(MantleLedgeWorldSpace);
	CalculateMantleTarget(MantleLedgeWorldSpace);
	CalculateMantleAnimatedStartOffset();
	PlayFromStartMantle(InMantleTimeline);
}

// Step 1: Get the Mantle Asset and use it to set the new Structs
void ACharacterBase::SetMantleParams(FMantleAsset InMantleAsset, const float InMantleHeight)
{
	MantleParams.AnimMontage = InMantleAsset.AnimMontage;
	MantleParams.Position = InMantleAsset.Position;
	MantleParams.StartingOffset = InMantleAsset.StartingOffset;

	const float LowHeight = InMantleAsset.LowHeight;
	const float LowPlayRate = InMantleAsset.LowPlayRate;
	const float LowStartPosition = InMantleAsset.LowStartPosition;
	const float HighHeight = InMantleAsset.HighHeight;
	const float HighPlayRate = InMantleAsset.HighPlayRate;
	const float HighStartPosition = InMantleAsset.HighStartPosition;

	MantleParams.PlayRate = UKismetMathLibrary::MapRangeClamped(InMantleHeight, LowHeight, HighHeight, LowPlayRate, HighPlayRate);
	MantleParams.StartingPosition = UKismetMathLibrary::MapRangeClamped(InMantleHeight, LowHeight, HighHeight, LowStartPosition, HighStartPosition);
}

// Step 2: Convert the world space target to the mantle component's local space for use in moving objects.
void ACharacterBase::SetMantleLedgeLocalSpace(const FLSComponentAndTransform MantleLedgeWorldSpace)
{
	MantleLedgeLS = ULocomotionSystemMacroLibrary::ComponentWorldToLocal(MantleLedgeWorldSpace);
}

// Step 3: Set the Mantle Target and calculate the Starting Offset (offset amount between the actor and target transform).
void ACharacterBase::CalculateMantleTarget(const FLSComponentAndTransform MantleLedgeWorldSpace)
{
	MantleTarget = MantleLedgeWorldSpace.Transform;
	MantleActualStartOffset = ULocomotionSystemMacroLibrary::TransformMinus(GetActorTransform(), MantleTarget);
}

// Step 4: Calculate the Animated Start Offset from the Target Location. 
// This would be the location the actual animation starts at relative to the Target Transform. 
void ACharacterBase::CalculateMantleAnimatedStartOffset()
{
	const FVector From = MantleTarget.GetLocation();
	FVector Rotate2Vector = UKismetMathLibrary::Conv_RotatorToVector(FRotator(MantleTarget.GetRotation()));
	Rotate2Vector *= MantleParams.StartingOffset.Y;

	const FVector To = FVector(Rotate2Vector.X, Rotate2Vector.Y, MantleParams.StartingOffset.Z);

	const FVector Diff = (From - To);
	FTransform FromTransform = FTransform::Identity;
	FromTransform.SetRotation(MantleTarget.GetRotation());
	FromTransform.SetLocation(Diff);
	MantleAnimatedStartOffset = ULocomotionSystemMacroLibrary::TransformMinus(FromTransform, MantleTarget);
}

// Step 5: Clear the Character Movement Mode and set the Movement State to Mantling
// Step 6: Configure the Mantle Timeline so that it is the same length as the Lerp/Correction curve minus the starting position, 
// Plays at the same speed as the animation. Then start the timeline.
// Step 7: Play the Anim Montaget if valid.
void ACharacterBase::PlayFromStartMantle(UTimelineComponent* const InMantleTimeline)
{
	if (!MantleParams.AnimMontage)
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr Mantle AnimMontage  : %s"), *FString(__FUNCTION__));
		return;
	}

	check(GetAnimInstance());
	check(InMantleTimeline);

	check(GetCharacterMovement());
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ILocomotionSystemPawn::Execute_SetALSMovementMode(this, ELSMovementMode::Mantling);

	float OutMinTime, OutMaxTime = ZERO_VALUE;
	MantleParams.Position->GetTimeRange(OutMinTime, OutMaxTime);

	InMantleTimeline->SetTimelineLength(OutMaxTime - MantleParams.StartingPosition);
	InMantleTimeline->SetPlayRate(MantleParams.PlayRate);
	InMantleTimeline->PlayFromStart();

	GetAnimInstance()->StopAllMontages(ZERO_VALUE);

	GetAnimInstance()->Montage_Play(
		MantleParams.AnimMontage, 
		MantleParams.PlayRate, 
		EMontagePlayReturnType::MontageLength, 
		MantleParams.StartingPosition, 
		true);
}
#pragma endregion

