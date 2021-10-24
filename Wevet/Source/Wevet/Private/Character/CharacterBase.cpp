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
#include "Perception/AISense_Hearing.h"
#include "Item/NakedWeapon.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	JumpMaxHoldTime = 0.5f;
	BaseTurnRate = 150.f;
	BaseLookUpRate = 150.f;
	RecoverHealthValue = 100;
	RecoverTimer = 2.0f;
	StanTimeOut = 30.f;
	MeleeAttackTimeOut = ZERO_VALUE;
	TakeDamageTimeOut = ZERO_VALUE;

	HeadBoneName   = HEAD_BONE;
	ChestBoneName  = CHEST_BONE;
	PelvisBoneName = PELVIS_BONE;

	HeadSocketName   = HEAD_SOCKET;
	ChestSocketName  = CHEST_SOCKET;
	PelvisSocketName = PELVIS_SOCKET;
	GiveDamageType = EGiveDamageType::None;
	RagdollPoseSnapshot = FName(TEXT("RagdollPose"));

	bAiming = false;
	bWasDied = false;
	bWasMoving = false;
	bDebugTrace = false;
	bEnableRecover = false;
	bEnableRagdoll = true;
	bRagdollOnGround = false;
	bWasMovementInput = false;

	// Water Surface
	Tags.Add(WATER_TAG);
	GetMesh()->ComponentTags.Add(WATER_LOCAL_TAG);

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

	// SetUp Movement
	GetMovementComponent()->NavAgentProps.AgentRadius = 42.f;
	GetMovementComponent()->NavAgentProps.AgentRadius = 192.f;

	// SetUp CharacterMovement
	GetCharacterMovement()->BrakingFrictionFactor = ZERO_VALUE;
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = 1;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = 1;
	GetCharacterMovement()->NavAgentProps.bCanFly = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->Buoyancy = 1.3f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = 1;

	// SetUp Mesh
	GetMesh()->SetCollisionProfileName(FName(TEXT("ALS_Character")));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetRelativeRotation(FQuat(FRotator(0.f, 270.f, 0.f)));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -97.f));
	GetMesh()->bMultiBodyOverlap = 1;

	// SetUp Collision
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("ALS_Character")));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->InitCapsuleSize(22.f, 90.0f);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(Wevet::ProjectFile::GetBulletImpactPath());
		BloodTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(Wevet::ProjectFile::GetFootStepPath());
		FootStepSoundAsset = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetNakedContainerPath());
		NakedWeapon = FindAsset.Object;
	}

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Enable Damage Frag
	SetCanBeDamaged(true);

	// Replicate
	SetReplicates(true);

	// Tags
	Tags.Add(CHARACTER_TAG);
	Tags.Add(DAMAGE_TAG);

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


void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	TargetCharacter = nullptr;

	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}

	// Clear MeleeTimer
	{
		FTimerManager& TimerManager = GetWorldTimerManager();
		if (TimerManager.IsTimerActive(MeleeAttackHundle))
		{
			TimerManager.ClearTimer(MeleeAttackHundle);
			TimerManager.ClearAllTimersForObject(this);
		}
	}

	// Clear StanTimer
	{
		FTimerManager& TimerManager = GetWorldTimerManager();
		if (TimerManager.IsTimerActive(StanHundle))
		{
			TimerManager.ClearTimer(StanHundle);
			TimerManager.ClearAllTimersForObject(this);
		}
	}

	// RemoveBind
	RemoveBindAll();

	ActionInfoPtr = nullptr;
	MeshComponents.Reset(0);
	IgnoreActors.Reset(0);
	GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &ACharacterBase::HitReceive);
	Super::EndPlay(EndPlayReason);
}


void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	IgnoreActors.Add(this);

	{
		MeshComponents = Wevet::ComponentExtension::GetComponentsArray<UMeshComponent>(this);
	}

	{
		GetMesh()->AddTickPrerequisiteActor(this);
		GetAudioComponent()->AddTickPrerequisiteActor(this);
		GetComboComponent()->AddTickPrerequisiteActor(this);
		GetPawnNoiseEmitterComponent()->AddTickPrerequisiteActor(this);
	}

	{
		ILocomotionSystemPawn::Execute_OnALSViewModeChange(this);
		ILocomotionSystemPawn::Execute_OnALSRotationModeChange(this);
		ILocomotionSystemPawn::Execute_OnALSStanceChange(this);
	}

	CreateWeaponInstance(NakedWeapon, [&](AAbstractWeapon* Weapon)
	{
		//
	});

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACharacterBase::HitReceive);
}


void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateEssentialVariables();
	UpdateCombatTimer(DeltaTime);

	if (bEnableRecover)
	{
		UpdateRecoverTimer(DeltaTime);
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
bool ACharacterBase::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	check(GetMesh());
	static const FName AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;
	const TArray<USkeletalMeshSocket*> Sockets = GetMesh()->SkeletalMesh->GetActiveSocketList();
	const int32 CollisionQuery = ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_Pawn);

	for (int i = 0; i < Sockets.Num(); ++i)
	{
		const FVector SocketLocation = GetMesh()->GetSocketLocation(Sockets[i]->SocketName);
		const bool bHitResult = GetWorld()->LineTraceSingleByObjectType(
			HitResult,
			ObserverLocation,
			SocketLocation,
			FCollisionObjectQueryParams(CollisionQuery),
			FCollisionQueryParams(AILineOfSight, true, IgnoreActor));

		++NumberOfLoSChecksPerformed;
		if (!bHitResult || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this)))
		{
			OutSeenLocation = SocketLocation;
			OutSightStrength = 1;
			//UE_LOG(LogWevetClient, Warning, TEXT("Socket Name: %s"), *Sockets[i]->SocketName.ToString());
			return true;
		}
	}

	const bool bHitResult = GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		ObserverLocation,
		GetActorLocation(),
		FCollisionObjectQueryParams(CollisionQuery),
		FCollisionQueryParams(AILineOfSight, true, IgnoreActor));

	++NumberOfLoSChecksPerformed;
	if (!bHitResult || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;
		return true;
	}

	OutSightStrength = 0;
	return false;
}


// Any CombatTimerUpdate
void ACharacterBase::UpdateCombatTimer(const float InDeltaTime)
{
	if (WasTakeDamagePlaying())
	{
		TakeDamageTimeOut -= InDeltaTime;
	}
	if (WasMeleeAttackPlaying())
	{
		MeleeAttackTimeOut -= InDeltaTime;
	}
	if (WasEquipWeaponPlaying())
	{
		EquipWeaponTimeOut -= InDeltaTime;
	}

}


void ACharacterBase::UpdateRecoverTimer(const float InDeltaTime)
{
	if (ICombatInstigator::Execute_IsDeath(this) || IsFullHealth())
	{
		return;
	}

	if (RecoverInterval >= RecoverTimer)
	{
		RecoverInterval = ZERO_VALUE;
		if (CharacterModel)
		{
			CharacterModel->DoRecover(RecoverHealthValue);
		}
	}
	else
	{
		RecoverInterval += InDeltaTime;
	}
}


float ACharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
#if WITH_EDITOR
	if (DamageEvent.DamageTypeClass->GetClass())
	{
		auto Class = DamageEvent.DamageTypeClass->GetClass();
		UE_LOG(LogWevetClient, Log, TEXT("DamageClass => %s, funcName => %s"), *Class->GetName(), *FString(__FUNCTION__));
	}
#endif


	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ICombatInstigator::Execute_IsDeath(this))
	{
		return ActualDamage;
	}

	CharacterModel->DoTakeDamage((int32)ActualDamage);
	const bool bWasDie = CharacterModel->CanDie();

	if (bWasDie)
	{
		CharacterModel->DoDie();
		ICombatInstigator::Execute_Die(this);
	}

	if (EventInstigator && 
		EventInstigator->GetPawn() && 
		EventInstigator->GetPawn()->GetClass()->ImplementsInterface(UCombatInstigator::StaticClass()))
	{
		ICombatInstigator::Execute_InfrictionDamage(EventInstigator->GetPawn(), this, bWasDie);
	}

	// if NakedWeapon Clear Collision
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Get()->ClearCollisionApply();
	}

	return ActualDamage;
}


#pragma region Interaction
void ACharacterBase::Pickup_Implementation(const EItemType InItemType, AActor* Actor)
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
			AAbstractWeapon* Weapon = Cast<AAbstractWeapon>(Actor);
			if (Weapon == nullptr)
			{
				return;
			}

			// Already Inventory ItemType
			if (WasSameWeaponType(Weapon))
			{
				Weapon = nullptr;
				return;
			}

			FWeaponItemInfo WeaponItemInfo = Weapon->GetWeaponItemInfo();
			WeaponFunc Callback = [&](AAbstractWeapon* InstanceWeapon)
			{
				if (InstanceWeapon)
				{
					InstanceWeapon->CopyWeaponItemInfo(&WeaponItemInfo);
				}
			};

			CreateWeaponInstance(Weapon->GetClass(), Callback);
			IInteractionItem::Execute_Release(Weapon, this);
			Actor = nullptr;
		}
		break;

		case EItemType::Health:
		{
			//
		}
		break;

		case EItemType::Ammos:
		{
			AAbstractItem* Item = Cast<AAbstractItem>(Actor);
			if (Item == nullptr)
			{
				return;
			}

			if (AAbstractWeapon* Weapon = FindByWeapon(Item->GetWeaponItemType()))
			{
				IWeaponInstigator::Execute_DoReplenishment(Weapon, Item->GetReplenishmentAmmo());
				IInteractionItem::Execute_Release(Item, this);
				Actor = nullptr;
			}
		}
		break;
	}
}

bool ACharacterBase::CanPickup_Implementation() const
{
	return false;
}

void ACharacterBase::OverlapActor_Implementation(AActor* Actor)
{
	if (PickupComponent)
	{
		PickupComponent->SetPickupActor(Actor);
	}
}

void ACharacterBase::Release_Implementation()
{

}
#pragma endregion


#pragma region Sound
void ACharacterBase::ReportNoise_Implementation(USoundBase* Sound, float Volume)
{
	if (Sound)
	{
		// @Temp
		const float HearingRange = GetCapsuleComponent()->GetScaledCapsuleRadius();
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		//MakeNoise(InVolume, this, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), DEFAULT_VOLUME, this, HearingRange);
	}
}

void ACharacterBase::FootStep_Implementation(USoundBase* Sound, float Volume)
{
	USoundBase* const InSound = Sound ? Sound : FootStepSoundAsset;
	if (InSound)
	{
		// @Temp
		const float HearingRange = GetCapsuleComponent()->GetScaledCapsuleRadius();
		const float Speed = GetVelocity().Size();
		const float InVolume = FMath::Clamp<float>((Speed / GetCharacterMovement()->MaxWalkSpeed), MIN_VOLUME, DEFAULT_VOLUME);
		//MakeNoise(InVolume, this, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InSound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), DEFAULT_VOLUME, this, HearingRange);
	}
}

// @NOTE
// WeapnBaseClass called UAISense_Hearing::ReportNoiseEvent
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


#pragma region Combat
FCombatDelegate* ACharacterBase::GetDeathDelegate()
{
	return &DeathDelegate;
}


FCombatDelegate* ACharacterBase::GetAliveDelegate()
{
	return &AliveDelegate;
}


FCombatOneDelegate* ACharacterBase::GetKillDelegate()
{
	return &KillDelegate;
}


float ACharacterBase::GetMeleeDistance_Implementation() const
{
	if (!CurrentWeapon.IsValid())
	{
		return ZERO_VALUE;
	}
	return CurrentWeapon.Get()->GetWeaponItemInfo().MeleeDistance;
}


void ACharacterBase::SetActionState_Implementation(const EAIActionState InAIActionState)
{
	if (ActionState != InAIActionState)
	{
		ActionState = InAIActionState;
		ICombatInstigator::Execute_OnActionStateChange(this);
	}
}


void ACharacterBase::OnActionStateChange_Implementation()
{
}


EAIActionState ACharacterBase::GetActionState_Implementation() const
{
	return ActionState;
}


bool ACharacterBase::CanStrike_Implementation() const
{
	if (!TargetCharacter || !CurrentWeapon.IsValid() || ICombatInstigator::Execute_IsDeath(TargetCharacter))
	{
		return false;
	}

	if (!CurrentWeapon.Get()->WasEquip())
	{
		return false;
	}

	const FVector TargetLocation = TargetCharacter->GetActorLocation();
	const float Distance = (GetActorLocation() - TargetLocation).Size();
	const bool bWasResult = (GetMeleeDistance_Implementation() > Distance);
	return bWasResult;
}


AActor* ACharacterBase::GetTarget_Implementation() const
{
	return TargetCharacter;
}


bool ACharacterBase::CanKillDealDamage_Implementation(const FName BoneName) const
{
	if (BoneName == HeadBoneName) 
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Super::GetMesh();
		const FReferenceSkeleton RefSkeleton = SkeletalMeshComponent->SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
		if (RefSkeleton.FindBoneIndex(BoneName) != INDEX_NONE)
		{
			return true;
		}
	} 
	return false;
}


AAbstractWeapon* ACharacterBase::GetCurrentWeapon_Implementation() const
{
	return CurrentWeapon.Get();
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


bool ACharacterBase::IsStan_Implementation() const
{
	return (ALSMovementMode == ELSMovementMode::Ragdoll);
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
	else
	{
		// damage...
	}
}


void ACharacterBase::Die_Implementation()
{
	if (bWasDied)
	{
		return;
	}

	TargetCharacter = nullptr;
	bWasDied = true;
	SetReplicateMovement(false);
	SetActorTickEnabled(false);

	TearOff();

	CurrentWeapon.Reset();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ReleaseAllWeaponInventory();
	ReleaseAllItemInventory();
	KillRagdollPhysics();
}


void ACharacterBase::Alive_Implementation()
{
	if (CharacterModel)
	{
		CharacterModel->DoAlive();
	}

	bWasDied = false;

	if (AliveDelegate.IsBound())
	{
		AliveDelegate.Broadcast();
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
		CurrentWeapon.Get()->WeaponActionDelegate.AddDynamic(this, &ACharacterBase::WeaponFireCallBack);
	}
}


void ACharacterBase::UnEquipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		ICombatInstigator::Execute_DoFireReleassed(this);
		CurrentWeapon.Get()->WeaponActionDelegate.RemoveDynamic(this, &ACharacterBase::WeaponFireCallBack);
		const FName SocketName(CurrentWeapon.Get()->GetWeaponItemInfo().UnEquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		CurrentWeapon.Get()->SetEquip(false);
	}
	CurrentWeapon.Reset();
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
		{
			//
		}
		break;

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


void ACharacterBase::DoReload_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoReload(CurrentWeapon.Get());
	}
}


FVector ACharacterBase::BulletTraceRelativeLocation_Implementation() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetMuzzleTransform().GetLocation();
	}
	return GetActorLocation();
}


FVector ACharacterBase::BulletTraceForwardLocation_Implementation() const
{
	if (CurrentWeapon.IsValid())
	{
		const FTransform MuzzleTransform = CurrentWeapon.Get()->GetMuzzleTransform();
		const FVector MuzzleLocation = MuzzleTransform.GetLocation();
		const FRotator MuzzleRotation = FRotator(MuzzleTransform.GetRotation());
		const float TraceDistance = CurrentWeapon.Get()->GetTraceDistance();
		return MuzzleLocation + (MuzzleRotation.Vector() * TraceDistance);
	}
	return GetActorForwardVector();
}


void ACharacterBase::FireActionMontage_Implementation(float& OutFireDuration)
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	const bool bHasAnimMontage = (ActionInfoPtr && ActionInfoPtr->FireMontage);
	if (!bHasAnimMontage)
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr FireActionMontage : %s"), *GetName());
		return;
	}

	if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->FireMontage))
	{
		OutFireDuration += PlayAnimMontage(ActionInfoPtr->FireMontage, MONTAGE_DELAY);
	}
}


void ACharacterBase::ReloadActionMontage_Implementation(float& OutReloadDuration)
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	const bool bHasAnimMontage = (ActionInfoPtr && ActionInfoPtr->ReloadMontage);
	if (!bHasAnimMontage)
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr ReloadActionMontage : %s"), *GetName());
		return;
	}

	if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->ReloadMontage))
	{
		OutReloadDuration += PlayAnimMontage(ActionInfoPtr->ReloadMontage);
	}
}


void ACharacterBase::NakedAction_Implementation(const ENakedWeaponTriggerType NakedWeaponTriggerType, const bool Enable, bool& FoundResult)
{
	if (CurrentWeapon.IsValid())
	{
		if (ANakedWeapon* NakedWeaponActor = Cast<ANakedWeapon>(CurrentWeapon.Get()))
		{
			NakedWeaponActor->NakedActionApply(NakedWeaponTriggerType, Enable, FoundResult);
		}
	}

	if (!FoundResult)
	{
		UE_LOG(LogWevetClient, Error, TEXT("Fail NakedAction => %s"), *FString(__FUNCTION__));
	}
}
#pragma endregion


#pragma region Brain
class UBehaviorTree* ACharacterBase::GetBehaviorTree_Implementation() const
{
	return BehaviorTree;
}


void ACharacterBase::DoSightReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew)
{

}


void ACharacterBase::DoHearReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew)
{

}


void ACharacterBase::DoPredictionReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus)
{

}


void ACharacterBase::DoDamageReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus)
{
}
#pragma endregion


#pragma region ALS_Getter
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

FCameraFOVParam ACharacterBase::GetCameraFOVParam_Implementation() const
{
	return CameraFOVParam;
}

FCameraTraceParam ACharacterBase::GetCameraTraceParam_Implementation() const
{
	// @NOTE
	// Blueprint Override
	FCameraTraceParam Result;
	return Result;
}

FTransform ACharacterBase::GetPivotTarget_Implementation() const
{
	return GetActorTransform();
}

FVector ACharacterBase::GetCameraTarget_Implementation() const
{
	return GetMesh()->GetSocketLocation(FName(TEXT("FPS_Socket")));
}

bool ACharacterBase::HasMovementInput_Implementation() const
{
	return bWasMovementInput;
}

bool ACharacterBase::HasMoving_Implementation() const
{
	return bWasMoving;
}

bool ACharacterBase::HasAiming_Implementation() const
{
	return bAiming;
}

bool ACharacterBase::HasDebugTrace_Implementation() const
{
	return bDebugTrace;
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
#pragma endregion


#pragma region ALS_Setter
void ACharacterBase::SetALSCharacterRotation_Implementation(const FRotator AddAmount)
{
	//UE_LOG(LogWevetClient, Log, TEXT("Rotation : %s"), *FString(__FUNCTION__));
}


void ACharacterBase::SetALSCameraShake_Implementation(TSubclassOf<class UCameraShakeBase> InShakeClass, const float InScale)
{
}


void ACharacterBase::SetALSMovementMode_Implementation(const ELSMovementMode NewALSMovementMode)
{
	if (NewALSMovementMode == ELSMovementMode::None)
	{
		return;
	}

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
	UpdateCharacterMovementSettings();

	if (Super::IsLocallyControlled())
	{
		switch(ALSPrevMovementMode)
		{
			case ELSMovementMode::Grounded:
			{
				JumpRotation = bWasMoving ? LastVelocityRotation : CharacterRotation;
				RotationOffset = 0.f;
			}
			break;
			case ELSMovementMode::Falling:
			{
				//
			}
			break;
			case ELSMovementMode::Ragdoll:
			{
				JumpRotation = CharacterRotation;
			}
			break;
			case ELSMovementMode::Swimming:
			{
				//
			}
			break;
			case ELSMovementMode::Mantling:
			{
				//
			}
			break;
		}
	}
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
	ILocomotionSystemPawn::Execute_SetALSMovementAction(GetAnimInstance(), ALSMovementAction);
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
	ILocomotionSystemPawn::Execute_OnALSRotationModeChange(this);
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


void ACharacterBase::SetWalkingSpeed_Implementation(const float InWalkingSpeed)
{
	WalkingSpeed = InWalkingSpeed;
	ILocomotionSystemPawn::Execute_SetWalkingSpeed(GetAnimInstance(), WalkingSpeed);
	UpdateCharacterMovementSettings();
}


void ACharacterBase::SetRunningSpeed_Implementation(const float InRunningSpeed)
{
	RunningSpeed = InRunningSpeed;
	ILocomotionSystemPawn::Execute_SetRunningSpeed(GetAnimInstance(), RunningSpeed);
	UpdateCharacterMovementSettings();
}


void ACharacterBase::SetSprintingSpeed_Implementation(const float InSprintingSpeed)
{
	SprintingSpeed = InSprintingSpeed;
	ILocomotionSystemPawn::Execute_SetSprintingSpeed(GetAnimInstance(), SprintingSpeed);
	UpdateCharacterMovementSettings();
}


void ACharacterBase::SetCrouchingSpeed_Implementation(const float InCrouchingSpeed)
{
	CrouchingSpeed = InCrouchingSpeed;
	ILocomotionSystemPawn::Execute_SetCrouchingSpeed(GetAnimInstance(), CrouchingSpeed);
	UpdateCharacterMovementSettings();
}


void ACharacterBase::SetSwimmingSpeed_Implementation(const float InSwimmingSpeed)
{
	SwimmingSpeed = InSwimmingSpeed;
	ILocomotionSystemPawn::Execute_SetSwimmingSpeed(GetAnimInstance(), SwimmingSpeed);
	UpdateCharacterMovementSettings();
}
#pragma endregion


#pragma region ALS_NotUseAPI
void ACharacterBase::SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState)
{
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
#pragma endregion


#pragma region Input
void ACharacterBase::Jump()
{
	if (ALSMovementAction != ELSMovementAction::None)
	{
		return;
	}

	switch (ALSMovementMode)
	{
		case ELSMovementMode::Grounded:
		{
			const bool bWasMantleFail = bWasMovementInput ? (!MantleCheck(GroundedTraceSettings)) : true;

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
		{
		}
		break;
		
		case ELSMovementMode::Ragdoll:
		{
		}
		break;

		case ELSMovementMode::Swimming:
		{
		}
		break;
	}

}

void ACharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ACharacterBase::Sprint()
{
	if (!bWasSprint)
	{
		bWasSprint = true;
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
	if (bWasSprint)
	{
		bWasSprint = false;
	}
}

void ACharacterBase::OnWalkAction()
{
	switch (ALSGait)
	{
		case ELSGait::Walking:
		{
			ILocomotionSystemPawn::Execute_SetALSGait(this, ELSGait::Running);
		}
		break;
		case ELSGait::Running:
		{
			ILocomotionSystemPawn::Execute_SetALSGait(this, ELSGait::Walking);
		}
		break;
		case ELSGait::Sprinting:
		break;
	}
}

void ACharacterBase::OnCrouch()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	const bool bWasCrouch = (ALSStance == ELSStance::Crouching);
	if (bWasCrouch)
	{
		Super::UnCrouch();
	}
	else
	{
		if (Super::CanCrouch())
		{
			Super::Crouch();
		}
	}
	UpdateCharacterMovementSettings();
}

void ACharacterBase::CrouchUpdate(const bool InCrouch)
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (!InCrouch)
	{
		Super::UnCrouch();
	}
	else
	{
		if (Super::CanCrouch())
		{
			Super::Crouch();
		}
	}
	UpdateCharacterMovementSettings();
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
#if false
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Dir, Value);
	}
#endif

	ForwardAxisValue = Value;
	MovementInputControl(true);
}

void ACharacterBase::MoveRight(float Value)
{
#if false
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Dir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Dir, Value);
	}
#endif

	RightAxisValue = Value;
	MovementInputControl(false);
}

void ACharacterBase::MeleeAttack(const bool InEnable)
{
	if (CurrentWeapon.IsValid())
	{
		//IWeaponInstigator::Execute_DoMeleeAttack(CurrentWeapon.Get(), InEnable);
	}
}

void ACharacterBase::PickupObjects()
{
	check(GetPickupComponent());

	AActor* Actor = GetPickupComponent()->GetPickupActor();
	if (Actor == nullptr)
	{
		return;
	}

	if (IInteractionItem* Interface = Cast<IInteractionItem>(Actor))
	{
		const EItemType ItemType = IInteractionItem::Execute_GetItemType(Actor);
		IInteractionPawn::Execute_Pickup(this, ItemType, Actor);
	}
}

void ACharacterBase::ReleaseObjects()
{
	IInteractionPawn::Execute_Release(this);
}
#pragma endregion


#pragma region Weapon
bool ACharacterBase::HasEquipWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->WasEquip();
	}
	return false;
}


bool ACharacterBase::HasEmptyWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->WasEmpty();
	}
	return false;
}


EWeaponItemType ACharacterBase::GetCurrentWeaponType() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetWeaponItemType();
	}
	return EWeaponItemType::None;
}

/// <summary>
/// Switch weapons.
/// </summary>
/// <returns></returns>
AAbstractWeapon* ACharacterBase::SwitchWeaponAction() const
{
	// @NOTE
	// Find HighPriorityWeapon
	// etx Pistol, Rifle
	bool FoundResult = false;
	GetInventoryComponent()->FindHighPriorityWeapon(FoundResult);
	if (FoundResult)
	{
		// Found Gun
		return GetInventoryComponent()->GetAvailableWeapon();
	}
	else
	{
		// Not Found Naked
		return GetInventoryComponent()->GetNakedWeapon();
	}
	return nullptr;
}


AAbstractWeapon* ACharacterBase::FindByWeapon(const EWeaponItemType WeaponItemType) const
{
	return GetInventoryComponent()->FindByWeapon(WeaponItemType);
}


const bool ACharacterBase::WasSameWeaponType(AAbstractWeapon* const Weapon)
{
	if (!Weapon)
	{
		return false;
	}

	if (AAbstractWeapon* const InWeapon = FindByWeapon(Weapon->GetWeaponItemType()))
	{
		return InWeapon->WasSameWeaponType(Weapon->GetWeaponItemType());
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


void ACharacterBase::ReleaseAllWeaponInventory()
{
	check(GetInventoryComponent());
	if (GetInventoryComponent()->EmptyWeaponInventory())
	{
		return;
	}

	auto SourceArray = GetInventoryComponent()->GetWeaponInventory();
	TArray<class AWorldItem*> DestArray(SourceArray);
	ReleaseItemToWorld(DestArray);
	GetInventoryComponent()->ClearWeaponInventory();
}


void ACharacterBase::ReleaseAllItemInventory()
{
	check(GetInventoryComponent());
	if (GetInventoryComponent()->EmptyItemInventory())
	{
		return;
	}

	auto SourceArray = GetInventoryComponent()->GetItemInventory();
	TArray<class AWorldItem*> DestArray(SourceArray);
	ReleaseItemToWorld(DestArray);
	GetInventoryComponent()->ClearItemInventory();
}


void ACharacterBase::ReleaseItemToWorld(TArray<class AWorldItem*> ItemArray)
{
	TArray<FVector> SpawnPoints;
	const FVector RelativePosition = GetMesh()->GetComponentLocation();
	const int32 ItemNum = ItemArray.Num();
	UWevetBlueprintFunctionLibrary::CircleSpawnPoints(ItemNum, DEFAULT_FORWARD_VECTOR, RelativePosition, SpawnPoints);
	for (int Index = 0; Index < ItemNum; ++Index)
	{
		AWorldItem* Item = ItemArray[Index];
		if (!Item)
		{
			continue;
		}
		const FVector Position = SpawnPoints[Index];
		const FTransform SpawnTransform = FTransform(GetActorRotation(), Position, FVector::OneVector);
		ReleaseItemToWorld(Item, SpawnTransform);
	}
}


void ACharacterBase::ReleaseItemToWorld(AWorldItem* Item, const FTransform& Transform)
{
	if (!Item)
	{
		return;
	}

	IInteractionItem::Execute_Release(Item, nullptr);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;
	AWorldItem* const ItemPtr = GetWorld()->SpawnActor<AWorldItem>(Item->GetClass(), Transform, SpawnParams);
	if (ItemPtr)
	{
#if WITH_EDITOR
		ItemPtr->SetFolderPath("/WorldItem");
#endif

		IInteractionItem::Execute_SpawnToWorld(ItemPtr);
	}
	Item = nullptr;
}


void ACharacterBase::WeaponFireCallBack(const bool InFiredAction)
{
	UE_LOG(LogWevetClient, Log, TEXT("WeaponFireCallBack => %s"), InFiredAction ? TEXT("true") : TEXT("false"));
}
#pragma endregion


#pragma region Montages
void ACharacterBase::EquipmentActionMontage()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (InventoryComponent->EmptyWeaponInventory())
	{
		return;
	}

	if (WasEquipWeaponPlaying())
	{
		// Already MontagePlaying
		return;
	}

	// AI already has a Pointer.
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	SetActionInfo(CurrentWeapon.Get()->GetWeaponItemType());
	const bool bHasAnimMontage = (ActionInfoPtr && ActionInfoPtr->EquipMontage);
	if (!bHasAnimMontage)
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr EquipmentActionMontage : %s"), *GetName());
		return;
	}

	if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->EquipMontage))
	{
		EquipWeaponTimeOut += PlayAnimMontage(ActionInfoPtr->EquipMontage, MONTAGE_DELAY);
	}
}


void ACharacterBase::UnEquipmentActionMontage()
{
	if (!HasEquipWeapon())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
		return;
	}

	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	const bool bHasAnimMontage = (ActionInfoPtr && ActionInfoPtr->UnEquipMontage);
	if (!bHasAnimMontage)
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr UnEquipmentActionMontage : %s"), *GetName());
		return;
	}

	if (!GetAnimInstance()->Montage_IsPlaying(ActionInfoPtr->UnEquipMontage))
	{
		EquipWeaponTimeOut += PlayAnimMontage(ActionInfoPtr->UnEquipMontage, MONTAGE_DELAY);
	}
}


void ACharacterBase::TakeDamageMontage(const bool InForcePlaying)
{
	if (!InForcePlaying)
	{
		const bool bWasAnyPlaying = (WasTakeDamagePlaying() || WasMeleeAttackPlaying());
		if (bWasAnyPlaying)
		{
			return;
		}
	}

	check(GetAnimInstance());
	const bool bWasCrouched = (ALSStance == ELSStance::Crouching);
	const FName NodeName = (bWasMoving || bWasCrouched) ? UPPER_BODY : FULL_BODY;
	GetAnimInstance()->TakeDamageAnimation(ActionInfoPtr, NodeName);
}
#pragma endregion


#pragma region Utils
UCharacterAnimInstanceBase* ACharacterBase::GetAnimInstance() const
{
	return Cast<UCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());
}


UIKAnimInstance* ACharacterBase::GetIKAnimInstance() const
{
	return Cast<UIKAnimInstance>(GetMesh()->GetPostProcessInstance());
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


const TArray<class AActor*>& ACharacterBase::GetIgnoreActors()
{
	return IgnoreActors;
}


EDrawDebugTrace::Type ACharacterBase::GetDrawDebugTrace() const
{
	return bDebugTrace ? EDrawDebugTrace::Type::ForOneFrame : EDrawDebugTrace::Type::None;
}


FTransform ACharacterBase::GetChestTransform() const
{
	if (GetMesh())
	{
		return GetMesh()->GetSocketTransform(ChestSocketName);
	}
	else
	{
		return FTransform::Identity;
	}
}


void ACharacterBase::SetActionInfo(const EWeaponItemType InWeaponItemType)
{
	if (GetAnimInstance())
	{
		ActionInfoPtr = GetAnimInstance()->GetActionInfo(InWeaponItemType);
	}
}


void ACharacterBase::SetEnableRecover(const bool InEnableRecover)
{
	bEnableRecover = InEnableRecover;
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
	SkelMesh->SetCollisionProfileName(FName(TEXT("Ragdoll")));
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
	if (ALSMovementMode == ELSMovementMode::Ragdoll)
	{
		return;
	}

	//ICombatInstigator::Execute_UnEquipment(this);
	ICombatInstigator::Execute_DoFireReleassed(this);

	Super::SetReplicateMovement(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// @NOTE
	// Ragdoll enums are not used because classes other than player cross the boundaries of the outside world.
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
	Super::SetReplicateMovement(true);
	GetCharacterMovement()->SetMovementMode(bRagdollOnGround ? EMovementMode::MOVE_Walking : EMovementMode::MOVE_Falling);
	GetCharacterMovement()->Velocity = RagdollVelocity;
	ILocomotionSystemPawn::Execute_PoseSnapShot(GetAnimInstance(), RagdollPoseSnapshot);

	if (bRagdollOnGround)
	{
		const FRotator Rotation = GetMesh()->GetSocketRotation(PelvisBoneName);
		const bool bGetUpFront = (Rotation.Roll > 0.0f) ? true : false;
		UE_LOG(LogWevetClient, Log, TEXT("Rotation %s"), *Rotation.ToString());
		ILocomotionSystemPawn::Execute_SetGetup(GetAnimInstance(), bGetUpFront);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}
#pragma endregion


void ACharacterBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	if (IDamageTypeInstigator* Interface = Cast<IDamageTypeInstigator>(Hit.GetActor()))
	{
		ICombatInstigator::Execute_HitEffectReceive(this, Hit, IDamageTypeInstigator::Execute_GetGiveDamageType(Hit.GetActor()));
	}
}


void ACharacterBase::RemoveBindAll()
{
	if (DeathDelegate.IsBound())
	{
		DeathDelegate.RemoveAll(this);
		DeathDelegate.Clear();
	}

	if (KillDelegate.IsBound())
	{
		KillDelegate.RemoveAll(this);
		KillDelegate.Clear();
	}

	if (AliveDelegate.IsBound())
	{
		AliveDelegate.RemoveAll(this);
		AliveDelegate.Clear();
	}
}


#pragma region ALSFunction
FVector ACharacterBase::ChooseVelocity() const
{
	if (ALSMovementMode == ELSMovementMode::Ragdoll)
	{
		return GetMesh()->GetPhysicsLinearVelocity(PelvisBoneName);
	}
	return Super::GetVelocity();
}


float ACharacterBase::ChooseMaxWalkSpeed() const
{
	if (ALSMovementMode == ELSMovementMode::Swimming)
	{
		return SwimmingSpeed;
	}

	float Speed = 0.0f;
	const float CrouchOffset = 50.f;
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
		switch (ALSGait)
		{
			case ELSGait::Walking:
			Speed = CrouchingSpeed - CrouchOffset;
			break;
			case ELSGait::Running:
			Speed = CrouchingSpeed - CrouchOffset;
			//Speed = CrouchingSpeed;
			break;
			case ELSGait::Sprinting:
			//Speed = CrouchingSpeed + CrouchOffset;
			Speed = CrouchingSpeed;
			break;
		}
	}
	return Speed;
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


void ACharacterBase::DoWhileALSMovementMode()
{
	switch (ALSMovementMode)
	{
		case ELSMovementMode::None:
		break;
		case ELSMovementMode::Grounded:
		case ELSMovementMode::Swimming:
		DoWhileGrounded();
		break;
		case ELSMovementMode::Falling:
		DoWhileMantling();
		break;
		case ELSMovementMode::Ragdoll:
		DoWhileRagdolling();
		break;
		case ELSMovementMode::Mantling:
		break;
	}
}


bool ACharacterBase::CanSprint() const
{
	if (ALSMovementMode == ELSMovementMode::Ragdoll)
	{
		return false;
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
	const FRotator RotateAmount = UKismetMathLibrary::NegateRotator(AddAmount);
	TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, RotateAmount);

	const FRotator RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;
	CharacterRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, RotateAmount);
	SetActorRotation(CharacterRotation);
}


void ACharacterBase::DoWhileGrounded()
{
	const bool bWasStanding = (ALSStance == ELSStance::Standing);

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


void ACharacterBase::DoWhileMantling()
{
	if (bWasMovementInput)
	{
		MantleCheck(FallingTraceSettings);
	}
}


void ACharacterBase::DoWhileRagdolling()
{
	FRotator ActorRotation = FRotator::ZeroRotator;
	FVector ActorLocation = FVector::ZeroVector;
	UpdateRagdollTransform(ActorRotation, ActorLocation);
	CalcurateRagdollParams(RagdollVelocity, RagdollLocation, ActorRotation, ActorLocation);
}


void ACharacterBase::UpdateRagdollTransform(FRotator& OutActorRotation, FVector& OutActorLocation)
{
	// Set the "stiffness" of the ragdoll based on the speed.
	// The faster the ragdoll moves, the more rigid the joint.
	const float Length = UKismetMathLibrary::VSize(ChooseVelocity());
	const float Value = UKismetMathLibrary::MapRangeClamped(Length, 0.0f, 1000.0f, 0.0f, 25000.0f);
	GetMesh()->SetAllMotorsAngularDriveParams(Value, 0.0f, 0.0f, false);

	// Ragdolls not locally controlled on the client will be pushed toward the replicated 'Ragdoll Location' vector. 
	// They will still simulate separately, but will end up in the same location.
	if (!Super::IsLocallyControlled())
	{
		const FVector BoneLocation = GetMesh()->GetSocketLocation(PelvisBoneName);
		const FVector Position = (RagdollLocation - BoneLocation) * 200.0f;
		GetMesh()->AddForce(Position, PelvisBoneName, true);
		return;
	}

	// If the fall is too fast, disable gravity to prevent the ragdoll from continuing to accelerate.
	// Stabilize the movement of the Ragdoll and prevent it from falling off the floor.
	const bool bWasGravity = (ChooseVelocity().Z < -4000.f);
	GetMesh()->SetEnableGravity(bWasGravity ? false : true);

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


void ACharacterBase::CalcurateRagdollParams(const FVector InRagdollVelocity, const FVector InRagdollLocation, const FRotator InActorRotation, const FVector InActorLocation)
{
	RagdollVelocity = InRagdollVelocity;
	RagdollLocation = InRagdollLocation;
	CharacterRotation = InActorRotation;
	TargetRotation = CharacterRotation;
	Super::SetActorLocationAndRotation(InActorLocation, CharacterRotation);
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
			const float RotationRate = CalculateRotationRate(SlowSpeed, 5.f, FastSpeed, 10.f);
			ApplyCharacterRotation(FRotator(0.0f, LastVelocityRotation.Yaw, 0.0f), true, RotationRate);
		}
		break;

		case ELSRotationMode::LookingDirection:
		{
			const float RotationRate = (bAiming) ? CalculateRotationRate(SlowSpeed, 15.f, FastSpeed, 15.f) : CalculateRotationRate(SlowSpeed, 10.f, FastSpeed, 15.f);
			ApplyCharacterRotation(Rotation, true, RotationRate);
		}
		break;
	}
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


void ACharacterBase::MovementInputControl(const bool bForwardAxis)
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


void ACharacterBase::GroundMovementInput(const bool bForwardAxis)
{
	FVector OutForwardVector;
	FVector OutRightVector;
	SetForwardOrRightVector(OutForwardVector, OutRightVector);

	if (bForwardAxis)
	{
		AddMovementInput(OutForwardVector, ForwardAxisValue);
	}
	else
	{
		AddMovementInput(OutRightVector, RightAxisValue);
	}
}


void ACharacterBase::RagdollMovementInput()
{
	FVector OutForwardVector;
	FVector OutRightVector;
	SetForwardOrRightVector(OutForwardVector, OutRightVector);
	const FVector Position = UKismetMathLibrary::Normal((OutForwardVector * ForwardAxisValue) + (OutRightVector * RightAxisValue));

	float Speed = 0.0f;
	switch (ALSGait)
	{
		case ELSGait::Walking:
		Speed = WALK_SPEED;
		break;
		case ELSGait::Running:
		Speed = RUN_SPEED;
		break;
		case ELSGait::Sprinting:
		Speed = SPRINT_SPEED;
		break;
	}

	const FVector Torque = Position * Speed;
	const FVector Result = FVector(Torque.X * -1.f, Torque.Y, Torque.Z);
	GetMesh()->AddTorqueInRadians(Result, PelvisBoneName, true);
	GetCharacterMovement()->AddInputVector(Position);
}


void ACharacterBase::CalculateActorTransformRagdoll(const FRotator InRagdollRotation, const FVector InRagdollLocation, FRotator& OutActorRotation, FVector& OutActorLocation)
{
	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector StartLocation(InRagdollLocation);
	const FVector EndLocation(InRagdollLocation.X, InRagdollLocation.Y, InRagdollLocation.Z - CapsuleHalfHeight);

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	const bool bResult = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
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
		const float One = 1.0f;
		const FVector CurrentVector = FVector(ChooseVelocity().X, ChooseVelocity().Y, 0.0f);
		bWasMoving = UKismetMathLibrary::NotEqual_VectorVector(CurrentVector, FVector::ZeroVector, One);
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
	check(GetCharacterMovement());
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
	const FVector Pos(Velocity.X, Velocity.Y, 0.0f);
	const float Size = UKismetMathLibrary::VSize(Pos);
	const float FastRange = UKismetMathLibrary::MapRangeClamped(Size, SlowSpeed, FastSpeed, SlowSpeedRate, FastSpeedRate);
	const float SlowRange = UKismetMathLibrary::MapRangeClamped(Size, ZERO_VALUE, SlowSpeed, ZERO_VALUE, SlowSpeedRate);

	if (RotationRateMultiplier != DEFAULT_VALUE)
	{
		RotationRateMultiplier = FMath::Clamp(RotationRateMultiplier + GetWorld()->GetDeltaSeconds(), 0.0f, DEFAULT_VALUE);
	}
	const float Value = (Size > SlowSpeed) ? FastRange : SlowRange;
	const float Result = Value * RotationRateMultiplier;
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
	check(GetCharacterMovement());
	switch (GetCharacterMovement()->MovementMode)
	{
		case EMovementMode::MOVE_None:
		case EMovementMode::MOVE_Custom:
		return ELSMovementMode::None;

		case EMovementMode::MOVE_Walking:
		case EMovementMode::MOVE_NavWalking:
		return ELSMovementMode::Grounded;
		
		case EMovementMode::MOVE_Falling:
		case EMovementMode::MOVE_Flying:
		return ELSMovementMode::Falling;

		case EMovementMode::MOVE_Swimming:
		return ELSMovementMode::Swimming;
	}
	return ELSMovementMode::None;
}


void ACharacterBase::ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed)
{
	TargetRotation = InTargetRotation;
	const FRotator RotateDiff = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation);
	RotationDifference = RotateDiff.Yaw;

	const float DeltaTime = GetWorld()->DeltaTimeSeconds;
	const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, InterpSpeed);
	CharacterRotation = bInterpRotation ? InterpRotation : TargetRotation;
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

	const float MaxAccelerationValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.2f);
	const float GroundFrictionValue = UKismetMathLibrary::MapRangeClamped(Velocity, RangeA, RangeB, 1.0f, 0.4f);
	GetCharacterMovement()->MaxAcceleration = RunningAcceleration * MaxAccelerationValue;
	GetCharacterMovement()->GroundFriction = RunningGroundFriction * GroundFrictionValue;
}
#pragma endregion


#pragma region MantleCore
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


void ACharacterBase::MantleUpdate(const float BlendIn, const float InPlaybackPosition)
{
	float PositionAlpha, XYCorrectionAlpha, ZCorrectionAlpha = ZERO_VALUE;
	SetMantleUpdateAlpha(InPlaybackPosition, PositionAlpha, XYCorrectionAlpha, ZCorrectionAlpha);
	const FTransform LerpedTarget = MakeMantleLerpedTarget(BlendIn, PositionAlpha, XYCorrectionAlpha, ZCorrectionAlpha);
	SetActorLocationAndRotation(LerpedTarget.GetLocation(), FRotator(FQuat(LerpedTarget.GetRotation())), false, false);
}


void ACharacterBase::MantleEnd()
{
	if (ICombatInstigator::Execute_IsDeath(this))
	{
		return;
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}


void ACharacterBase::SetActorLocationAndRotation(const FVector NewLocation, const FRotator NewRotation, const bool bWasSweep, const bool bWasTeleport)
{
	TargetRotation = NewRotation;
	Super::SetActorLocationAndRotation(NewLocation, NewRotation, bWasSweep /*nullptr, ETeleportType::None*/);
}


const bool ACharacterBase::MantleCheck(const FMantleTraceSettings InTraceSetting)
{
	FVector TracePoint = FVector::ZeroVector;
	FVector TraceNormal = FVector::ZeroVector;
	FVector DownTraceLocation = FVector::ZeroVector;
	UPrimitiveComponent* HitComponent = nullptr;
	FTransform TargetTransform = FTransform::Identity;
	float MantleHeight = 0.0f;
	bool OutHitResult = false;

	TraceForwardToFindWall(InTraceSetting, TracePoint, TraceNormal, OutHitResult);
	if (!OutHitResult)
	{
		return OutHitResult;
	}

	SphereTraceByMantleCheck(InTraceSetting, TracePoint, TraceNormal, OutHitResult, DownTraceLocation, HitComponent);
	if (!OutHitResult)
	{
		return OutHitResult;
	}

	ConvertMantleHeight(DownTraceLocation, TraceNormal, OutHitResult, TargetTransform, MantleHeight);
	if (!OutHitResult)
	{
		return OutHitResult;
	}

	FLSComponentAndTransform WS;
	WS.Component = HitComponent;
	WS.Transform = TargetTransform;
	BP_MantleStart(MantleHeight, WS, GetMantleType(MantleHeight));
	return OutHitResult;
}
#pragma endregion


#pragma region MantleUtils
FVector ACharacterBase::GetCapsuleBaseLocation(const float ZOffset) const
{
	float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	HalfHeight += ZOffset;
	const FVector ComponentUp = GetCapsuleComponent()->GetUpVector() * HalfHeight;
	const FVector ComponentLocation = GetCapsuleComponent()->GetComponentLocation();
	return (ComponentLocation - ComponentUp);
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
	check(GetCapsuleComponent());
	const float InvRadiusOffset = RadiusOffset * -1.f;
	const float Hemisphere = GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere();
	const float Offset = Hemisphere + InvRadiusOffset + HeightOffset;

	// Editor Settings
	const FName ProfileName(TEXT("ALS_Character"));
	const FVector OffsetPosition = FVector(0.0f, 0.0f, Offset);
	const FVector StartLocation = (TargetLocation + OffsetPosition);
	const FVector EndLocation = (TargetLocation - OffsetPosition);
	const float TraceRadius = (GetCapsuleComponent()->GetScaledCapsuleRadius() + RadiusOffset);

	FHitResult HitData(ForceInit);
	TArray<AActor*> Ignore;
	const float DrawTime = 1.0f;

	const bool bWasHitResult = UKismetSystemLibrary::SphereTraceSingleByProfile(
		GetWorld(),
		StartLocation,
		EndLocation,
		TraceRadius,
		ProfileName,
		false,
		Ignore,
		GetDrawDebugTrace(),
		HitData,
		true, 
		FLinearColor::Green,
		FLinearColor::FLinearColor(0.9f, 0.3f, 1.0f, 1.0),
		DrawTime);

	return !(HitData.bBlockingHit || HitData.bStartPenetrating);
}
#pragma endregion


#pragma region MantleSystem
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

	TArray<AActor*> Ignore;
	const float DrawTime = 1.0f;

	const bool bWasHitResult = UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(), 
		StartLocation, 
		EndLocation, 
		Radius, 
		HalfHeight,
		TraceType,
		false,		// TraceComplex
		Ignore,
		GetDrawDebugTrace(),
		HitData,
		true, 
		FLinearColor::Black, 
		FLinearColor::Black, 
		DrawTime);

	const bool bWalkableHit = !(GetCharacterMovement()->IsWalkable(HitData));
	const bool bBlockingHit = HitData.bBlockingHit;
	const bool bInitialOverlap = !(HitData.bStartPenetrating);

	OutHitResult = (bWalkableHit && bBlockingHit && bInitialOverlap);

	if (OutHitResult)
	{
		OutInitialTraceImpactPoint = HitData.ImpactPoint;
		OutInitialTraceNormal = HitData.ImpactNormal;
	}
}

// step2 Trace downward from the first trace's Impact Point and determine if the hit location is walkable.
void ACharacterBase::SphereTraceByMantleCheck(
	const FMantleTraceSettings TraceSetting, 
	const FVector InitialTraceImpactPoint, 
	const FVector InitialTraceNormal, 
	bool& OutHitResult, 
	FVector& OutDownTraceLocation,
	UPrimitiveComponent*& OutPrimitiveComponent)
{
	const FVector CapsuleLocation = GetCapsuleBaseLocation(2.0f);
	FVector TraceNormal = InitialTraceNormal * -15.f;
	FVector ImpactPoint = InitialTraceImpactPoint;
	ImpactPoint.Z = CapsuleLocation.Z;

	float MaxLedgeHeight = TraceSetting.MaxLedgeHeight;
	MaxLedgeHeight += TraceSetting.DownwardTraceRadius;
	MaxLedgeHeight += 1.0f;

	FVector PreStartLocation = FVector::ZeroVector;
	PreStartLocation.Z = MaxLedgeHeight;

	const FVector StartLocation = (ImpactPoint + TraceNormal + PreStartLocation);
	const FVector EndLocation = (ImpactPoint + TraceNormal);
	const float Radius = TraceSetting.DownwardTraceRadius;
	
	// @NOTE
	// Access For LedgeTrace 
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);
	FHitResult HitData(ForceInit);
	TArray<AActor*> Ignore;
	const float DrawTime = 1.0f;

	const bool bWasHitResult = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(), 
		StartLocation, 
		EndLocation, 
		Radius, 
		TraceType, 
		false, 
		Ignore,
		GetDrawDebugTrace(), 
		HitData, 
		true, 
		FLinearColor::Yellow, 
		FLinearColor::Red, 
		DrawTime);

	const bool bWalkableHit = GetCharacterMovement()->IsWalkable(HitData);
	const bool bResult = HitData.bBlockingHit;

	OutHitResult = (bWalkableHit && bResult);
	OutDownTraceLocation = FVector(HitData.Location.X, HitData.Location.Y, HitData.ImpactPoint.Z);
	OutPrimitiveComponent = HitData.Component.Get();
}

// step3 Check if the capsule has room to stand at the downward trace's location. 
// If so, set that location as the Target Transform and calculate the mantle height.
void ACharacterBase::ConvertMantleHeight(
	const FVector DownTraceLocation, 
	const FVector InitialTraceNormal, 
	bool& OutRoomCheck, 
	FTransform& OutTargetTransform,
	float& OutMantleHeight)
{
	const float ZOffset = 20.0f;
	const FVector RelativeLocation = GetCapsuleLocationFromBase(DownTraceLocation, ZOffset);
	const FVector Offset = FVector(-1.0f, -1.0f, 0.0f);

	// DisplayName RotationFromXVector
	const FRotator RelativeRotation = UKismetMathLibrary::Conv_VectorToRotator(InitialTraceNormal * Offset);
	
	// Result bool
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
#if WITH_EDITOR
	UE_LOG(LogWevetClient, Log, TEXT("InMantleHeight => %f"), InMantleHeight);
#endif

	EMantleType Current = EMantleType::HighMantle;
	switch (ALSMovementMode)
	{
		case ELSMovementMode::Falling:
		{
			Current = EMantleType::FallingCatch;
		}
		break;

		case ELSMovementMode::None:
		case ELSMovementMode::Grounded:
		case ELSMovementMode::Ragdoll:
		case ELSMovementMode::Swimming:
		case ELSMovementMode::Mantling:
		{
			const float LowBorder = 125.f;
			if (InMantleHeight < LowBorder)
			{
				Current = EMantleType::LowMantle;
			}
		}
		break;
	}
	return Current;
}
#pragma endregion


#pragma region MantleUpdate
// Step 1: Continually update the mantle target from the stored local transform to follow along with moving objects.
void ACharacterBase::SetMantleTarget()
{
	auto Component = ULocomotionSystemMacroLibrary::ComponentLocalToWorld(MantleLedgeLS);
	MantleTarget = Component.Transform;
}

// Step 2: Update the Position and Correction Alphas using the Position/Correction curve set for each Mantle.
void ACharacterBase::SetMantleUpdateAlpha(const float PlaybackPoint, float& OutPositionAlpha, float& OutXY, float& OutZ)
{
	check(MantleParams.Position);
	const FVector CurveVector = MantleParams.Position->GetVectorValue(MantleParams.StartingPosition + PlaybackPoint);
	OutPositionAlpha = CurveVector.X;
	OutXY = CurveVector.Y;
	OutZ = CurveVector.Z;
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
FTransform ACharacterBase::MakeMantleTransform(const float Position, const float XY, const float Z) const
{
	const FTransform A = MakeXYCollectionAlphaTransform(XY);
	const FTransform B = MakeZCollectionAlphaTransform(Z);
	FTransform BlendTransform = FTransform::Identity;
	const FQuat RotA = A.GetRotation();
	FVector PosA = A.GetLocation();
	FVector PosB = B.GetLocation();
	PosA.Z = PosB.Z;
	BlendTransform.SetRotation(RotA);
	BlendTransform.SetLocation(PosA);
	FTransform Result = ULocomotionSystemMacroLibrary::TransformPlus(MantleTarget, BlendTransform);
	return UKismetMathLibrary::TLerp(Result, MantleTarget, Position);
}

// Step 3: Lerp multiple transforms together for independent control over the horizontal and vertical blend to the animated start position, 
// as well as the target position.
FTransform ACharacterBase::MakeMantleLerpedTarget(const float Blend, const float Position, const float XY, const float Z) const
{
	const FTransform Result = ULocomotionSystemMacroLibrary::TransformPlus(MantleTarget, MantleActualStartOffset);
	const FTransform MantleTransform = MakeMantleTransform(Position, XY, Z);
	return UKismetMathLibrary::TLerp(Result, MantleTransform, Blend);
}
#pragma endregion


#pragma region MantleStart
// Step 1: Get the Mantle Asset and use it to set the new Structs
void ACharacterBase::SetMantleParams(FMantleAsset InMantleAsset, const float InMantleHeight)
{
	const float LowHeight = InMantleAsset.LowHeight;
	const float LowPlayRate = InMantleAsset.LowPlayRate;
	const float LowStartPosition = InMantleAsset.LowStartPosition;
	const float HighHeight = InMantleAsset.HighHeight;
	const float HighPlayRate = InMantleAsset.HighPlayRate;
	const float HighStartPosition = InMantleAsset.HighStartPosition;

	MantleParams.AnimMontage = InMantleAsset.AnimMontage;
	MantleParams.Position = InMantleAsset.Position;
	MantleParams.StartingOffset = InMantleAsset.StartingOffset;

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
	FVector Rotate2Vector = UKismetMathLibrary::Conv_RotatorToVector(FRotator(MantleTarget.GetRotation()));
	Rotate2Vector *= MantleParams.StartingOffset.Y;

	const FVector From = MantleTarget.GetLocation();
	const FVector To = FVector(Rotate2Vector.X, Rotate2Vector.Y, MantleParams.StartingOffset.Z);

	FTransform FromTransform = FTransform::Identity;
	FromTransform.SetRotation(MantleTarget.GetRotation());
	FromTransform.SetLocation(From - To);
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

	//GetAnimInstance()->StopAllMontages(ZERO_VALUE);
	const float PlayRate = MantleParams.PlayRate;
	const float StartPosition = MantleParams.StartingPosition;
	const EMontagePlayReturnType MontageTypes = EMontagePlayReturnType::MontageLength;
	GetAnimInstance()->Montage_Play(MantleParams.AnimMontage, PlayRate, MontageTypes, StartPosition, false);
}
#pragma endregion

