// Copyright 2018 wevet works All Rights Reserved.

#include "Character/CharacterBase.h"
#include "Item/ItemBase.h"
#include "Character/CharacterModel.h"
#include "AnimInstance/CharacterAnimInstanceBase.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"

using namespace Wevet;

DEFINE_LOG_CATEGORY(LogWevetClient);

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	MovementSpeed(300.f),
	TakeDamageInterval(0.f),
	ComboTakeInterval(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	HeadBoneName = FName(TEXT("Head"));
	HeadSocketName = FName(TEXT("Head_Socket"));
	PelvisSocketName = FName(TEXT("Pelvis_Socket"));
	ChestSocketName = FName(TEXT("Chest_Socket"));

	bCrouch  = false;
	bSprint  = false;
	bWasDied = false;
	bHanging = false;
	bClimbingLedge = false;
	bClimbJumping = false;

	PawnNoiseEmitterComponent = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("PawnNoiseEmitterComponent"));
	PawnNoiseEmitterComponent->bAutoActivate = 1;
	PawnNoiseEmitterComponent->bAutoRegister = 1;

	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->SetupAttachment(GetMesh());

	PickupComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterPickupComponent>(this, TEXT("PickupComponent"));
	PickupComponent->bAutoActivate = 1;
	PickupComponent->bAutoRegister = 1;

	InventoryComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterInventoryComponent>(this, TEXT("InventoryComponent"));
	InventoryComponent->bAutoActivate = 1;
	InventoryComponent->bAutoRegister = 1;

	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = 1;
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetCharacterMovement())
	{
		DefaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
}

void ACharacterBase::BeginDestroy()
{
	if (CharacterModel && CharacterModel->IsValidLowLevel())
	{
		CharacterModel->ConditionalBeginDestroy();
	}
	Super::BeginDestroy();
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyZero(TakeDamageInterval))
	{
		TakeDamageInterval -= DeltaTime;
	}
	if (!FMath::IsNearlyZero(ComboTakeInterval))
	{
		ComboTakeInterval -= DeltaTime;
	}
}

float ACharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

#pragma region InteractionExecuter
void ACharacterBase::OnReleaseItemExecuter_Implementation() 
{
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Picking : %s"), *(Actor->GetName()));
	}
}

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
	IGrabInstigator::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
}

void ACharacterBase::ClimbLedge_Implementation(bool InClimbLedge)
{
}

void ACharacterBase::ReportClimbEnd_Implementation()
{
	bHanging = false;
	bClimbingLedge = false;
	IGrabInstigator::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
	IGrabInstigator::Execute_ClimbLedge(GetCharacterAnimInstance(), bClimbingLedge);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ACharacterBase::ClimbMove_Implementation(float Value)
{
	IGrabInstigator::Execute_ClimbMove(GetCharacterAnimInstance(), Value);
}

void ACharacterBase::ClimbJump_Implementation()
{
	IGrabInstigator::Execute_ClimbJump(GetCharacterAnimInstance());
}

void ACharacterBase::ReportClimbJumpEnd_Implementation()
{
	bHanging = true;
	bCanClimbJumpLeft = false;
	bCanClimbJumpRight = false;
	//bClimbJumping = false;
	IGrabInstigator::Execute_ClimbJump(GetCharacterAnimInstance());
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
bool ACharacterBase::IsDeath_Implementation()
{
	if (bWasDied || CharacterModel == nullptr)
	{
		return true;
	}
	return CharacterModel->IsDie();
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied)
{
	if (BoneName == HeadBoneName) 
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Super::GetMesh();
		auto RefSkeleton = SkeletalMeshComponent->SkeletalMesh->Skeleton->GetReferenceSkeleton();
		if (RefSkeleton.FindBoneIndex(BoneName) != INDEX_NONE)
		{
			CharacterModel->SetHealth(INDEX_NONE);
		}
	} 
	else
	{
		CharacterModel->TakeDamage((int32)Damage);
	}

	bDied = CharacterModel->IsEmptyHealth();
	if (bDied)
	{
		CharacterModel->Die();
		Die_Implementation();
	}
	else
	{
		TakeDamageActionMontage();
	}
}

void ACharacterBase::ApplyDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage)
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

	bWasDied = true;
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}

	USkeletalMeshComponent* const SkelMesh = GetMesh();
	SkelMesh->SetAllBodiesSimulatePhysics(true);
	SkelMesh->SetSimulatePhysics(true);
	SkelMesh->WakeAllRigidBodies();
	SkelMesh->bBlendPhysics = true;

	Super::GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UWorld* const World = GetWorld();
	check(World);
	const FVector ForwardOffset = Controller ? Controller->GetControlRotation().Vector() : Super::GetActorForwardVector();
	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward   = Super::GetActorLocation() + (ForwardOffset * DEFAULT_FORWARD_VECTOR);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (Controller)
	{
		Controller->UnPossess();
	}

	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}

	for (AAbstractWeapon*& Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (!Weapon)
		{
			continue;
		}
		ReleaseWeaponToWorld(Transform, Weapon);
	}
	InventoryComponent->ClearWeaponInventory();
	InventoryComponent->ConditionalBeginDestroy();
	InventoryComponent = nullptr;
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
#pragma endregion

FVector ACharacterBase::BulletTraceRelativeLocation() const
{
	return FVector::ZeroVector;
}

FVector ACharacterBase::BulletTraceForwardLocation() const
{
	return FVector::ZeroVector;
}

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
	MovementSpeed = bSprint ? DefaultMaxSpeed : DefaultMaxSpeed * HALF_WEIGHT;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	bCrouch = !bCrouch;
	Super::bIsCrouched = bCrouch ? 1 : 0;
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

float ACharacterBase::GetHealthToWidget() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->GetHealthToWidget();
	}
	return ZERO_VALUE;
}

bool ACharacterBase::IsHealthHalf() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->IsHealthHalf();
	}
	return false;
}

bool ACharacterBase::IsHealthQuarter() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->IsHealthQuarter();
	}
	return false;
}

void ACharacterBase::ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon* &Weapon)
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}
	const FWeaponItemInfo WeaponItemInfo = Weapon->WeaponItemInfo;
	TSubclassOf<class AAbstractWeapon> WeaponClass = Weapon->GetClass();
	IInteractionInstigator::Execute_Release(Weapon, nullptr);
	Weapon = nullptr;

	AAbstractWeapon* const SpawningObject = World->SpawnActorDeferred<AAbstractWeapon>(
		WeaponClass,
		Transform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
	SpawningObject->FinishSpawning(Transform);
}

AAbstractWeapon* ACharacterBase::FindByWeapon(const EWeaponItemType WeaponItemType)
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return nullptr;
	}
	for (AAbstractWeapon*& Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (Weapon && Weapon->WasSameWeaponType(WeaponItemType))
		{
			return Weapon;
		}
	}
	return nullptr;
}

UCharacterAnimInstanceBase* ACharacterBase::GetCharacterAnimInstance() const
{
	return Cast<UCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());
}

bool ACharacterBase::HasCrouch() const
{
	return bCrouch;
}

bool ACharacterBase::HasSprint() const
{
	return bSprint;
}

bool ACharacterBase::HasHanging() const
{
	return bHanging;
}

bool ACharacterBase::HasClimbingLedge() const
{
	return bClimbingLedge;
}

bool ACharacterBase::HasClimbingMoveLeft() const
{
	return bCanClimbMoveLeft;
}

bool ACharacterBase::HasClimbingMoveRight() const
{
	return bCanClimbMoveRight;
}

void ACharacterBase::PickupObjects()
{
}

void ACharacterBase::ReleaseObjects()
{
}

#pragma region Weapon
AAbstractWeapon* ACharacterBase::GetSelectedWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get();
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

AAbstractWeapon* ACharacterBase::GetUnEquipWeapon()
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return nullptr;
	}
	for (AAbstractWeapon*& Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (Weapon && !Weapon->WasEquip())
		{
			return Weapon;
		}
	}
	return nullptr;
}

void ACharacterBase::OutUnEquipWeaponList(TArray<AAbstractWeapon*>& OutWeaponList)
{
	if (InventoryComponent->HasInventoryWeaponItems())
	{
		return;
	}
	for (AAbstractWeapon* &Weapon : InventoryComponent->GetWeaponInventoryOriginal())
	{
		if (Weapon && !Weapon->WasEquip())
		{
			OutWeaponList.Emplace(Weapon);
		}
	}
}

const bool ACharacterBase::SameWeapon(AAbstractWeapon* const Weapon)
{
	const EWeaponItemType ItemType = Weapon->WeaponItemInfo.WeaponItemType;
	if (AAbstractWeapon * const InWeapon = FindByWeapon(ItemType))
	{
		return InWeapon->WasSameWeaponType(ItemType);
	}
	return false;
}
#pragma endregion

// Only Player override
void ACharacterBase::EquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeaponItems())
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
			PlayAnimMontage(ActionInfo.EquipMontage, MONTAGE_DELAY);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr AnimMontage : %s"), *FString(__FUNCTION__));
		}
	}
}

void ACharacterBase::UnEquipmentActionMontage()
{
	if (InventoryComponent->HasInventoryWeaponItems())
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
			PlayAnimMontage(ActionInfo.UnEquipMontage, MONTAGE_DELAY);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr AnimMontage : %s"), *FString(__FUNCTION__));
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
			UE_LOG(LogWevetClient, Error, TEXT("nullptr AnimMontage : %s"), *FString(__FUNCTION__));
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
			OutReloadDuration += PlayAnimMontage(ActionInfo.ReloadMontage);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr AnimMontage : %s"), *FString(__FUNCTION__));
		}
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("Not Equiped Weapon : %s"), *FString(__FUNCTION__));
	}
}

void ACharacterBase::TakeDamageActionMontage()
{
	if (!FMath::IsNearlyZero(TakeDamageInterval))
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
			TakeDamageInterval = PlayAnimMontage(ActionInfo.HitDamageMontage);
		}
	}
	else
	{
		if (DefaultHitDamageMontage)
		{
			TakeDamageInterval = PlayAnimMontage(DefaultHitDamageMontage);
		}
	}
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

void ACharacterBase::CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InTemplate, bool bSetEquip)
{
	if (InTemplate == nullptr)
	{
		return;
	}

	AAbstractWeapon* const SpawningObject = GetWorld()->SpawnActorDeferred<AAbstractWeapon>(
		InTemplate,
		GetActorTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	FWeaponItemInfo WeaponItemInfo = SpawningObject->WeaponItemInfo;
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
	SpawningObject->FinishSpawning(GetActorTransform());
	SpawningObject->AttachToComponent(Super::GetMesh(), Rules, WeaponItemInfo.UnEquipSocketName);
	InventoryComponent->AddWeaponInventory(SpawningObject);
	IInteractionInstigator::Execute_Take(SpawningObject, this);

	if (bSetEquip)
	{
		CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(SpawningObject);
	}
}

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

