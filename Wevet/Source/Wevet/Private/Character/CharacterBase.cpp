// Copyright 2018 wevet works All Rights Reserved.

#include "Character/CharacterBase.h"
#include "AnimInstance/CharacterAnimInstanceBase.h"
#include "AnimInstance/IKAnimInstance.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	TakeDamageInterval(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	HeadBoneName = HEAD_BONE;
	PelvisBoneName = PELVIS_BONE;
	HeadSocketName   = HEAD_SOCKET;
	PelvisSocketName = PELVIS_SOCKET;
	ChestSocketName  = CHEST_SOCKET;

	bCrouch  = false;
	bSprint  = false;
	bWasDied = false;
	bHanging = false;
	bClimbingLedge = false;
	bClimbJumping = false;
	bEnableRagdoll = true;
	bDebugTrace = false;

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

	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("Pawn")));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/VFX/Cascade/Gameplay/Blood/P_body_bullet_impact"));
		BloodTemplate = FindAsset.Object;
	}

	SetCanBeDamaged(true);
	Tags.Add(FName(TEXT("Character")));

	// @NOTE ALS
	WalkingSpeed = 200.f;
	RunningSpeed = 400.f;
	SprintingSpeed = 800.f;
	CrouchingSpeed = 200.f;
	WalkingAcceleration = 800.f;
	RunningAcceleration = 1000.f;
	WalkingDeceleration = 800.f;
	RunningDeceleration = 800.f;
	WalkingGroundFriction = 8.f;
	RunningGroundFriction = 6.f;
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
	GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &ACharacterBase::HitReceive);
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	// @NOTE
	// Ž©g‚ÌŒã‚ÉTick‚³‚¹‚é
	GetMesh()->AddTickPrerequisiteActor(this);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACharacterBase::HitReceive);
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TakeDamageInterval >= ZERO_VALUE)
	{
		TakeDamageInterval -= DeltaTime;
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
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	USoundBase* const InSound = Sound ? Sound : FootStepSoundAsset;
	if (InSound)
	{
		//MakeNoise(InVolume, this, GetActorLocation());
		const float Speed = GetVelocity().Size();
		const float InVolume = FMath::Clamp<float>((Speed / GetCharacterMovement()->MaxWalkSpeed), MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(World, InSound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (Sound)
	{
		const float InVolume = FMath::Clamp<float>(Volume, MIN_VOLUME, DEFAULT_VOLUME);
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(Actor, InVolume, Location);
	}
}
#pragma endregion

#pragma region IGrabInstigator
void ACharacterBase::CanGrab_Implementation(bool InCanGrab)
{
	bHanging = InCanGrab;
	GetCharacterMovement()->SetMovementMode(bHanging ? EMovementMode::MOVE_Flying : EMovementMode::MOVE_Walking);
	IGrabInstigator::Execute_CanGrab(GetAnimInstance(), bHanging);
}

void ACharacterBase::ClimbLedge_Implementation(bool InClimbLedge)
{
}

void ACharacterBase::ReportClimbEnd_Implementation()
{
	bHanging = false;
	bClimbingLedge = false;
	IGrabInstigator::Execute_CanGrab(GetAnimInstance(), bHanging);
	IGrabInstigator::Execute_ClimbLedge(GetAnimInstance(), bClimbingLedge);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ACharacterBase::ClimbMove_Implementation(float Value)
{
	IGrabInstigator::Execute_ClimbMove(GetAnimInstance(), Value);
}

void ACharacterBase::ClimbJump_Implementation()
{
	IGrabInstigator::Execute_ClimbJump(GetAnimInstance());
}

void ACharacterBase::ReportClimbJumpEnd_Implementation()
{
	if (GetAnimInstance())
	{
		GetAnimInstance()->StopAllMontages(ZERO_VALUE);
	}
	bHanging = true;
	bCanClimbJumpLeft = false;
	bCanClimbJumpRight = false;
	//bClimbJumping = false;
	IGrabInstigator::Execute_ClimbJump(GetAnimInstance());
}

void ACharacterBase::TurnConerLeftUpdate_Implementation()
{
}

void ACharacterBase::TurnConerRightUpdate_Implementation()
{
}

void ACharacterBase::TurnConerResult_Implementation()
{
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

	const bool bWasDeath = CharacterModel->IsEmptyHealth();
	if (!bWasDeath)
	{
		TakeDamageActionMontage();
	}
	else
	{
		CharacterModel->Die();
		IDamageInstigator::Execute_Die(this);
	}
	IDamageInstigator::Execute_InfrictionDamage(EventInstigator->GetPawn(), this, bWasDeath);
	return ActualDamage;
}

bool ACharacterBase::IsDeath_Implementation()
{
	if (bWasDied || CharacterModel == nullptr)
	{
		return true;
	}
	return CharacterModel->IsDie();
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
	//	
}

void ACharacterBase::Die_Implementation()
{
	if (bWasDied)
	{
		return;
	}
	SetReplicateMovement(false);
	TearOff();
	bWasDied = true;
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	const FVector ForwardOffset = Controller ? Controller->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward   = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (InventoryComponent && !InventoryComponent->HasInventoryWeapon())
	{
		for (AAbstractWeapon* Weapon : InventoryComponent->GetWeaponInventory())
		{
			if (!Weapon)
			{
				continue;
			}
			ReleaseWeaponToWorld(Transform, Weapon);
		}
	}
	InventoryComponent->ClearWeaponInventory();
	if (bEnableRagdoll)
	{
		SetRagdollPhysics();
	}
}

void ACharacterBase::Equipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Get()->SetEquip(true);
	}
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Get()->SetEquip(false);
	}
}

UCharacterModel* ACharacterBase::GetPropertyModel_Implementation() const
{
	return CharacterModel;
}

// @NOTE
// BloodEffect‚ð•\Ž¦
void ACharacterBase::HitEffectReceive_Implementation(const FHitResult& HitResult)
{
	FTransform Transform;
	Transform.SetIdentity();
	Transform.SetLocation(HitResult.ImpactPoint);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodTemplate, Transform, true);
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

#pragma region CharacterAction
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

void ACharacterBase::OnSprint()
{
	bSprint = !bSprint;

	// now crouching slow speed
	if (bCrouch)
	{
		bSprint = false;
	}
	GetCharacterMovement()->MaxWalkSpeed = bSprint ? SprintingSpeed : RunningSpeed;
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
		return CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
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
	const EWeaponItemType ItemType = Weapon->WeaponItemInfo.WeaponItemType;
	if (AAbstractWeapon* const InWeapon = FindByWeapon(ItemType))
	{
		return InWeapon->WasSameWeaponType(ItemType);
	}
	return false;
}

void ACharacterBase::CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, bool bSetEquip)
{
	if (InWeaponTemplate == nullptr)
	{
		return;
	}

	AAbstractWeapon* const SpawningObject = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<AAbstractWeapon>(this, InWeaponTemplate, GetActorTransform(), this);

	if (!SpawningObject)
	{
		return;
	}

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	SpawningObject->CopyWeaponItemInfo(&SpawningObject->WeaponItemInfo);
	SpawningObject->FinishSpawning(GetActorTransform());
	SpawningObject->AttachToComponent(Super::GetMesh(), Rules, SpawningObject->WeaponItemInfo.UnEquipSocketName);
	InventoryComponent->AddWeaponInventory(SpawningObject);
	IInteractionInstigator::Execute_Take(SpawningObject, this);

	if (bSetEquip)
	{
		CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(SpawningObject);
	}
}

void ACharacterBase::ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon)
{
	if (Weapon == nullptr)
	{
		return;
	}

	const FWeaponItemInfo* WeaponItemInfo = &Weapon->WeaponItemInfo;
	IInteractionInstigator::Execute_Release(Weapon, nullptr);
	AAbstractWeapon* const SpawningObject = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<AAbstractWeapon>(this, Weapon->GetClass(), Transform, nullptr);
	if (SpawningObject)
	{
		SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
		SpawningObject->FinishSpawning(Transform);
	}
	Weapon = nullptr;
}
#pragma endregion

#pragma region Montages
void ACharacterBase::EquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeapon())
	{
		return;
	}

	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		FWeaponActionInfo ActionInfo;
		SetActionInfo(WeaponType, ActionInfo);

		if (ActionInfo.EquipMontage)
		{
			if (GetAnimInstance()->Montage_IsPlaying(ActionInfo.EquipMontage))
			{
				return;
			}
			PlayAnimMontage(ActionInfo.EquipMontage, MONTAGE_DELAY);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr EquipMontage : %s"), *FString(__FUNCTION__));
		}
	}
}

void ACharacterBase::UnEquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeapon())
	{
		return;
	}

	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		FWeaponActionInfo ActionInfo;
		SetActionInfo(WeaponType, ActionInfo);

		if (ActionInfo.UnEquipMontage)
		{
			if (GetAnimInstance()->Montage_IsPlaying(ActionInfo.UnEquipMontage))
			{
				return;
			}
			PlayAnimMontage(ActionInfo.UnEquipMontage, MONTAGE_DELAY);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr UnEquipMontage : %s"), *FString(__FUNCTION__));
		}
	}
}

void ACharacterBase::FireActionMontage()
{
	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		FWeaponActionInfo ActionInfo;
		SetActionInfo(WeaponType, ActionInfo);

		if (ActionInfo.FireMontage)
		{
			PlayAnimMontage(ActionInfo.FireMontage);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr FireActionMontage : %s"), *FString(__FUNCTION__));
		}
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
	}
}

void ACharacterBase::ReloadActionMontage(float& OutReloadDuration)
{
	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		FWeaponActionInfo ActionInfo;
		SetActionInfo(WeaponType, ActionInfo);

		if (ActionInfo.ReloadMontage)
		{
			if (GetAnimInstance()->Montage_IsPlaying(ActionInfo.ReloadMontage))
			{
				return;
			}
			OutReloadDuration += PlayAnimMontage(ActionInfo.ReloadMontage);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr ReloadActionMontage : %s"), *FString(__FUNCTION__));
		}
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
	}
}

void ACharacterBase::TakeDamageActionMontage()
{
	if (TakeDamageInterval >= ZERO_VALUE)
	{
		return;
	}

	if (CurrentWeapon.IsValid())
	{
		const EWeaponItemType WeaponType = CurrentWeapon.Get()->WeaponItemInfo.WeaponItemType;
		FWeaponActionInfo ActionInfo;
		SetActionInfo(WeaponType, ActionInfo);

		if (ActionInfo.HitDamageMontage)
		{
			if (GetAnimInstance()->Montage_IsPlaying(ActionInfo.HitDamageMontage))
			{
				return;
			}
			TakeDamageInterval = PlayAnimMontage(ActionInfo.HitDamageMontage);
		}
	}
	else
	{
		if (DefaultHitDamageMontage)
		{
			if (GetAnimInstance()->Montage_IsPlaying(DefaultHitDamageMontage))
			{
				return;
			}
			TakeDamageInterval = PlayAnimMontage(DefaultHitDamageMontage);
		}
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

void ACharacterBase::SetActionInfo(const EWeaponItemType InWeaponItemType, FWeaponActionInfo& OutWeaponActionInfo)
{
	if (Wevet::ArrayExtension::NullOrEmpty(ActionInfoArray))
	{
		return;
	}

	for (FWeaponActionInfo& Info : ActionInfoArray)
	{
		if (InWeaponItemType == Info.WeaponItemType)
		{
			OutWeaponActionInfo = Info;
			break;
		}
	}
}

void ACharacterBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ABulletBase* Bullet = Cast<ABulletBase>(Hit.GetActor()))
	{
		UE_LOG(LogWevetClient, Log, TEXT("Hit : %s, Self : %s, Func : %s"), *Hit.GetActor()->GetName(), *GetName(), *FString(__FUNCTION__));

		// @NOTE
		// Ž€‚ñ‚Å‚àBloodEffect‚Í•\Ž¦
		FTransform Transform;
		Transform.SetIdentity();
		Transform.SetLocation(Hit.ImpactPoint);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodTemplate, Transform, true);
	}
}

void ACharacterBase::SetRagdollPhysics()
{
	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkelMesh->SetCollisionProfileName(TEXT("BlockAll"));
	SkelMesh->SetAllBodiesSimulatePhysics(true);
	//SkelMesh->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	SkelMesh->SetSimulatePhysics(true);
	SkelMesh->WakeAllRigidBodies();
	SkelMesh->bBlendPhysics = 1;
	SkelMesh->bIgnoreRadialForce = 1;
	SkelMesh->bIgnoreRadialImpulse = 1;
}

void ACharacterBase::SetForwardOrRightVector(FVector& OutForwardVector, FVector& OutRightVector)
{
	FRotator Rotation = FRotator::ZeroRotator;
	auto PawnRotation = GetControlRotation();
	Rotation.Yaw = PawnRotation.Yaw;
	OutForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);
	OutRightVector = UKismetMathLibrary::GetRightVector(Rotation);
}

