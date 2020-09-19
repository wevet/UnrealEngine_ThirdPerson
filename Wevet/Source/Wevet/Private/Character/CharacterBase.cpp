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

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	TakeDamageTimeOut(0.f),
	MeleeAttackTimeOut(0.f),
	StanTimeOut(30.f)
{
	PrimaryActorTick.bCanEverTick = true;

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

	// @NOTE ALS
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

	static ConstructorHelpers::FObjectFinder<UCurveVector> LowVectorAsset(TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/MantleCurves/Mantle_1m"));
	static ConstructorHelpers::FObjectFinder<UCurveVector> HighVectorAsset(TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/MantleCurves/Mantle_2m"));

	// DefaultMantleLow
	DefaultLowMantleAsset.StartingOffset = FVector(0.f, 65.f, 100.f);
	DefaultLowMantleAsset.LowHeight = 50.f;
	DefaultLowMantleAsset.LowPlayRate = 1.f;
	DefaultLowMantleAsset.LowStartPosition = 0.6f;
	DefaultLowMantleAsset.HighHeight = 100.f;
	DefaultLowMantleAsset.HighPlayRate = 1.f;
	DefaultLowMantleAsset.HighStartPosition = 0.0f;
	DefaultLowMantleAsset.Position = LowVectorAsset.Object;

	// DefaultMantleHigh
	DefaultHighMantleAsset.StartingOffset = FVector(0.f, 65.f, 200.f);
	DefaultHighMantleAsset.LowHeight = 125.f;
	DefaultHighMantleAsset.LowPlayRate = 1.2f;
	DefaultHighMantleAsset.LowStartPosition = 0.6f;
	DefaultHighMantleAsset.HighHeight = 200.f;
	DefaultHighMantleAsset.HighPlayRate = 1.2f;
	DefaultHighMantleAsset.HighStartPosition = 0.0f;
	DefaultHighMantleAsset.Position = HighVectorAsset.Object;

}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
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

	FTimerManager& TimerManager = GetWorldTimerManager();
	if (TimerManager.IsTimerActive(MeleeAttackHundle))
	{
		TimerManager.ClearTimer(MeleeAttackHundle);
		TimerManager.ClearAllTimersForObject(this);
	}
	if (TimerManager.IsTimerActive(StanHundle))
	{
		TimerManager.ClearTimer(StanHundle);
		TimerManager.ClearAllTimersForObject(this);
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
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

#pragma region InteractionPawn
void ACharacterBase::Pickup_Implementation(const EItemType InItemType, AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Picking : %s"), *(Actor->GetName()));
	}
}

const bool ACharacterBase::CanPickup_Implementation()
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
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (Sound)
	{
		//MakeNoise(Volume, this, GetActorLocation());
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(World, Sound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
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

	if (!bDeath)
	{
		//TakeDamageActionMontage();
	}
	else
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

void ACharacterBase::MakeDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage)
{
	const int32 BaseAttack = CharacterModel->GetAttack() + InWeaponDamage;
	const int32 Attack = BaseAttack;
	const int32 Deffence = DamageModel->GetDefence();
	const int32 Wisdom = DamageModel->GetWisdom();
	const int32 TotalDamage = (Attack - (Deffence + Wisdom)) / DEFFENCE_CONST;
	float Damage = (float)TotalDamage;
	OutDamage = FMath::Abs(Damage);
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
		FireReleassed();
		const FName SocketName(CurrentWeapon.Get()->GetWeaponItemInfo().UnEquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		CurrentWeapon.Get()->SetEquip(false);
	}
	ActionInfoPtr = nullptr;
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
			TakeDamageActionMontage(false);
		}
		break;
		case EGiveDamageType::Melee:
		{
			TakeDamageActionMontage(true);
			StartRagdollAction();
		}
		break;
	}
}
#pragma endregion

#pragma region Model
float ACharacterBase::GetHealthToWidget() const
{
	if (CharacterModel)
	{
		return CharacterModel->GetHealthToWidget();
	}
	return ZERO_VALUE;
}

bool ACharacterBase::IsFullHealth() const
{
	if (CharacterModel)
	{
		return CharacterModel->IsFullHealth();
	}
	return false;
}

bool ACharacterBase::IsHealthHalf() const
{
	if (CharacterModel)
	{
		return CharacterModel->IsHealthHalf();
	}
	return false;
}

bool ACharacterBase::IsHealthQuarter() const
{
	if (CharacterModel)
	{
		return CharacterModel->IsHealthQuarter();
	}
	return false;
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
}

void ACharacterBase::ReleaseObjects()
{
}

void ACharacterBase::FirePressed()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoFirePressed(CurrentWeapon.Get());
	}
}

void ACharacterBase::FireReleassed()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoFireRelease(CurrentWeapon.Get());
	}
}

void ACharacterBase::MeleeAttack()
{
	if (CurrentWeapon.IsValid())
	{
		MeleeAttackMontage();
	}
}

void ACharacterBase::Reload()
{
	if (CurrentWeapon.IsValid())
	{
		IWeaponInstigator::Execute_DoReload(CurrentWeapon.Get());
	}
}

void ACharacterBase::ReleaseWeapon()
{
}
#pragma endregion

#pragma region Weapon
AAbstractWeapon* ACharacterBase::FindByWeapon(const EWeaponItemType WeaponItemType) const
{
	if (InventoryComponent->HasInventoryWeapon())
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

AAbstractWeapon* ACharacterBase::GetNotEmptyWeapon() const
{
	if (InventoryComponent->HasInventoryWeapon())
	{
		return nullptr;
	}
	for (AAbstractWeapon* Weapon : InventoryComponent->GetWeaponInventory())
	{
		if (!Weapon->WasEmpty())
		{
			return Weapon;
		}
	}
	return nullptr;
}

void ACharacterBase::CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, TFunction<void(AAbstractWeapon* Weapon)> Callback)
{
	if (!InWeaponTemplate)
	{
		if (Callback)
			Callback(nullptr);
		return;
	}

	AAbstractWeapon* const SpawningObject = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<AAbstractWeapon>(this, InWeaponTemplate, GetActorTransform(), this);
	if (!SpawningObject)
	{
		if (Callback)
			Callback(nullptr);
		return;
	}

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	SpawningObject->FinishSpawning(GetActorTransform());
	SpawningObject->AttachToComponent(Super::GetMesh(), Rules, SpawningObject->GetWeaponItemInfo().UnEquipSocketName);
	InventoryComponent->AddWeaponInventory(SpawningObject);
	IInteractionInstigator::Execute_Take(SpawningObject, this);

	if (Callback)
		Callback(SpawningObject);
}

void ACharacterBase::ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon)
{
	if (!Weapon)
	{
		return;
	}

	FWeaponItemInfo WeaponItemInfo = Weapon->GetWeaponItemInfo();
	IInteractionInstigator::Execute_Release(Weapon, nullptr);

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
	if (!InventoryComponent || InventoryComponent->HasInventoryWeapon())
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
	if (InventoryComponent->HasInventoryWeapon())
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

void ACharacterBase::TakeDamageActionMontage(const bool InForcePlaying)
{
	if (!InForcePlaying)
	{
		if (WasTakeDamagePlaying() || WasMeleeAttackPlaying())
		{
			return;
		}
	}

	UAnimMontage* SelectMontage = ActionInfoPtr ? ActionInfoPtr->HitDamageMontage : DefaultHitDamageMontage;
	if (SelectMontage && !GetAnimInstance()->Montage_IsPlaying(SelectMontage))
	{
		TakeDamageTimeOut += PlayAnimMontage(SelectMontage);
	}
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

		MeleeAttackTimeOut = PlayAnimMontage(ActionInfoPtr->MeleeAttackMontage, DEFAULT_VALUE);
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
#pragma endregion

#pragma region Ragdoll
void ACharacterBase::KillRagdollPhysics()
{
	if (!bEnableRagdoll)
	{
		return;
	}
	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkelMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	SkelMesh->SetAllBodiesSimulatePhysics(true);
	SkelMesh->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	SkelMesh->WakeAllRigidBodies();
	SkelMesh->bBlendPhysics = 1;
	SkelMesh->bIgnoreRadialForce = 1;
	SkelMesh->bIgnoreRadialImpulse = 1;
	//SkelMesh->SetAllMassScale(50000.f);
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
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	//SkelMesh->SetCollisionProfileName(TEXT("Ragdoll"));

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		RagdollToWakeUpAction();
	});
	GetWorld()->GetTimerManager().SetTimer(StanHundle, TimerCallback, StanTimeOut, false);
}

void ACharacterBase::RagdollToWakeUpAction()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	const FRotator Rotation = GetMesh()->GetSocketRotation(PelvisBoneName);
	GetAnimInstance()->RagdollToWakeUpAction((Rotation.Roll > 0.0f));

	{
		GetAnimInstance()->SavePoseSnapshot(RagdollPoseSnapshot);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetAllBodiesSimulatePhysics(false);
	//SkelMesh->SetCollisionProfileName(TEXT("BlockAll"));
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
	if (Wevet::ArrayExtension::NullOrEmpty(ActionInfoArray))
		return;

	for (FWeaponActionInfo& Info : ActionInfoArray)
	{
		if (InWeaponItemType == Info.WeaponItemType)
		{
			ActionInfoPtr = &Info;
			break;
		}
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

FMantleAsset ACharacterBase::GetMantleAsset(const EMantleType InMantleType) const
{
	FMantleAsset Temp;
	switch (InMantleType)
	{
		case EMantleType::LowMantle:
		Temp = DefaultLowMantleAsset;
		break;

		case EMantleType::FallingCatch:
		case EMantleType::HighMantle:
		Temp = DefaultHighMantleAsset;
		break;
	}
	return Temp;
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
void ACharacterBase::SphereTraceByMantleCheck(
	const FMantleTraceSettings TraceSetting,
	const FVector InitialTraceImpactPoint,
	const FVector InitialTraceNormal,
	bool& OutHitResult,
	FVector& OutDownTraceLocation,
	UPrimitiveComponent*& OutPrimitiveComponent)
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

