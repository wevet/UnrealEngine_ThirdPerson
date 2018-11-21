// Copyright 2018 wevet works All Rights Reserved.

#include "WeaponBase.h"
#include "CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponBase::AWeaponBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	CharacterOwner(nullptr), 
	WidgetComponent(nullptr),
	SphereComponent(nullptr),
	SkeletalMeshComponent(nullptr),
	MuzzleSocketName(FName(TEXT("MuzzleFlash")))
{
	PrimaryActorTick.bCanEverTick = true;
	this->BulletDuration = 0.1F;
	this->ReloadDuration = 2.0f;

	// setup scene component
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent  = SceneComponent;

	// set skeletal
	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	// parent component at skeletalmeshcomponent
	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(SkeletalMeshComponent);

	// weapon umg
	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(180.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(SkeletalMeshComponent);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	if (WidgetComponent && WidgetComponent->IsValidLowLevel()) 
	{
		WidgetComponent->SetVisibility(false);
	}
	if (SphereComponent && SphereComponent->IsValidLowLevel())
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::EndOverlapRecieve);
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World && World->GetTimerManager().IsTimerActive(this->ReloadTimerHandle))
	{
		World->GetTimerManager().ClearTimer(this->ReloadTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void AWeaponBase::OnEquip(const bool Equip)
{
	this->Equip = Equip;
}

void AWeaponBase::SetPickable(const bool Pick)
{
	this->CanPick = Pick;
}

void AWeaponBase::SetReload(const bool Reload)
{
	this->IsReload = Reload;
}

void AWeaponBase::OnFirePress_Implementation()
{
	this->CanFired = true;
}

void AWeaponBase::OnFireRelease_Implementation()
{
	this->CanFired = false;
}

void AWeaponBase::OffVisible_Implementation()
{
	this->SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->SkeletalMeshComponent->SetSimulatePhysics(false);
	this->WidgetComponent->SetVisibility(false);
}

void AWeaponBase::OnVisible_Implementation()
{
	this->SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->SkeletalMeshComponent->SetSimulatePhysics(true);
	this->WidgetComponent->SetVisibility(true);
}

void AWeaponBase::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (this->CharacterOwner == nullptr)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
		if (Character)
		{
			SetCharacterOwner(Character);
		}
	}

	if (this->WidgetComponent)
	{
		this->WidgetComponent->SetVisibility(true);
	}
	if (this->SkeletalMeshComponent)
	{
		this->SkeletalMeshComponent->SetRenderCustomDepth(true);
	}
	SetPickable(true);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		Super::EnableInput(PlayerController);
	}
}

void AWeaponBase::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (this->CharacterOwner == nullptr)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
		if (Character)
		{
			SetCharacterOwner(Character);
		}
	}

	if (this->WidgetComponent)
	{
		this->WidgetComponent->SetVisibility(false);
	}
	if (this->SkeletalMeshComponent)
	{
		this->SkeletalMeshComponent->SetRenderCustomDepth(false);
	}
	SetPickable(false);
}

void AWeaponBase::OnFirePressedInternal()
{
	UWorld* World = GetWorld();

	// not found owner
	if (World == nullptr  
		|| this->CharacterOwner == nullptr
		|| (this->CharacterOwner && this->CharacterOwner->IsDeath_Implementation()))
	{
		return;
	}

	// current weapon reloading...
	if (this->IsReload)
	{
		return;
	}

	// empty current ammo
	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Out Of Ammos"));
		OnReloading_Implementation();
		return;
	}

	const float ForwardOffset = 15000.f;
	const FVector ForwardLocation = this->CharacterOwner->BulletTraceForwardLocation();
	const FVector StartLocation   = this->CharacterOwner->BulletTraceRelativeLocation();
	const FVector EndLocation     = StartLocation + (ForwardLocation * ForwardOffset);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams fCollisionQueryParams;
	fCollisionQueryParams.TraceTag = FName("");
	fCollisionQueryParams.OwnerTag = FName("");
	fCollisionQueryParams.bTraceAsyncScene = false;
	fCollisionQueryParams.bTraceComplex = true;
	fCollisionQueryParams.bFindInitialOverlaps = false;
	fCollisionQueryParams.bReturnFaceIndex = false;
	fCollisionQueryParams.bReturnPhysicalMaterial = false;
	fCollisionQueryParams.bIgnoreBlocks = false;
	fCollisionQueryParams.IgnoreMask = 0;
	fCollisionQueryParams.AddIgnoredActor(this);

	bool bSuccess = World->LineTraceSingleByChannel(
		HitData, 
		StartLocation, 
		EndLocation, 
		ECollisionChannel::ECC_Visibility, 
		fCollisionQueryParams);

	FTransform MuzzleTransform = GetSkeletalMeshComponent()->GetSocketTransform(GetMuzzleSocket());
	const FVector MuzzleLocation  = MuzzleTransform.GetLocation();
	const FRotator MuzzleRotation = FRotator(MuzzleTransform.GetRotation());

	UGameplayStatics::PlaySoundAtLocation(World, FireSoundAsset, MuzzleLocation, 1.f, 1.f, 0.f, nullptr, nullptr);
	this->CharacterOwner->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	UGameplayStatics::PlaySoundAtLocation(World, FireImpactSoundAsset, HitData.Location, 1.f, 1.f, 0.f, nullptr, nullptr);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint   = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation  = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* Bullet = World->SpawnActor<ABulletBase>(this->BulletsBP, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	if (HitData.Actor.IsValid())
	{
		ICombat* CombatInterface = Cast<ICombat>(HitData.Actor);
		if (bSuccess && CombatInterface)
		{
			if (CombatInterface->IsDeath_Implementation())
			{
				CombatInterface->Die_Implementation();
			}
			else
			{
				// @TODO
				// owner Character Set
				float Damage = FMath::FRandRange(10.f, 20.f);
				CombatInterface->OnTakeDamage_Implementation(
					HitData.BoneName, 
					Damage, 
					this);
			}
		}
	}

	// spawn impact emitter
	const FTransform EmitterTransform = UKismetMathLibrary::MakeTransform(HitData.Location, FRotator::ZeroRotator, FVector::ZeroVector);
	UParticleSystemComponent* ImpactMetalEmitterComponent = UGameplayStatics::SpawnEmitterAtLocation(
		World, 
		ImpactMetalEmitterTemplate, 
		EmitterTransform, 
		true);

	// attach muzzleflash emitter
	UParticleSystemComponent* MuzzleFlashEmitterComponent = UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlashEmitterTemplate, 
		GetSkeletalMeshComponent(),
		GetMuzzleSocket(), 
		MuzzleLocation,
		MuzzleRotation,
		EAttachLocation::KeepWorldPosition, 
		true);
}


void AWeaponBase::OnReloading_Implementation()
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (WeaponItemInfo.MaxAmmo == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Empty Ammos"));
		return;
	}

	if (WeaponItemInfo.CurrentAmmo >= WeaponItemInfo.ClipType)
	{
		UE_LOG(LogTemp, Warning, TEXT("Full Ammos Current:%d, ClipType:%d"), 
			WeaponItemInfo.CurrentAmmo, 
			WeaponItemInfo.ClipType);
		return;
	}

	SetReload(true);
	OnReloadInternal();
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([this]
	{
		SetReload(false);
	});
	World->GetTimerManager().SetTimer(this->ReloadTimerHandle, TimerCallback, this->ReloadDuration, false);
}


void AWeaponBase::OnFireReleaseInternal()
{
}

void AWeaponBase::OnReloadInternal()
{
	this->CanFired = false;
	this->CharacterOwner->ReloadActionMontage();
	this->NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);

	if (WeaponItemInfo.MaxAmmo <= this->NeededAmmo)
	{
		WeaponItemInfo.MaxAmmo = 0;
		WeaponItemInfo.CurrentAmmo = (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo);
	}
	else
	{
		WeaponItemInfo.MaxAmmo = (WeaponItemInfo.MaxAmmo - this->NeededAmmo);
		WeaponItemInfo.CurrentAmmo = WeaponItemInfo.ClipType;
	}
}

void AWeaponBase::SetCharacterOwner(ACharacterBase* InCharacterOwner)
{
	this->CharacterOwner = InCharacterOwner;
}
