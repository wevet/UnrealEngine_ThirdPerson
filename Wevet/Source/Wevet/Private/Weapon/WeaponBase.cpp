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
	MuzzleSocketName(FName(TEXT("MuzzleFlash"))),
	BulletDuration(0.1f),
	ReloadDuration(2.f),
	bEmpty(false)
{
	PrimaryActorTick.bCanEverTick = true;

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

	if (ensure(WidgetComponent)) 
	{
		WidgetComponent->SetVisibility(false);
	}

	if (ensure(SphereComponent)) 
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
	if (World && World->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void AWeaponBase::SetEquip(const bool Equip)
{
	bEquip = Equip;
}

void AWeaponBase::SetReload(const bool Reload)
{
	bReload = Reload;
}

void AWeaponBase::OnFirePress_Implementation()
{
	bFired = true;
}

void AWeaponBase::OnFireRelease_Implementation()
{
	bFired = false;
}

void AWeaponBase::OffVisible_Implementation()
{
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->SetSimulatePhysics(false);
	WidgetComponent->SetVisibility(false);
}

void AWeaponBase::OnVisible_Implementation()
{
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SkeletalMeshComponent->SetSimulatePhysics(true);
	WidgetComponent->SetVisibility(true);
}

void AWeaponBase::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (CharacterOwner == nullptr)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
		if (Character)
		{
			SetCharacterOwner(Character);
		}
	}

	WidgetComponent->SetVisibility(true);
	SkeletalMeshComponent->SetRenderCustomDepth(true);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		Super::EnableInput(PlayerController);
	}
}

void AWeaponBase::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CharacterOwner == nullptr)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
		if (Character)
		{
			SetCharacterOwner(Character);
		}
	}

	WidgetComponent->SetVisibility(false);
	SkeletalMeshComponent->SetRenderCustomDepth(false);
}

void AWeaponBase::OnFirePressedInternal()
{
	UWorld* const World = GetWorld();

	// not found owner
	if (World == nullptr  
		|| CharacterOwner == nullptr
		|| (CharacterOwner && CharacterOwner->IsDeath_Implementation()))
	{
		return;
	}

	if (bEmpty || bReload)
	{
		return;
	}

	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Out Of Ammos"));
		OnReloading_Implementation();
		return;
	}

	const float ForwardOffset = 15000.f;
	const FVector ForwardLocation = CharacterOwner->BulletTraceForwardLocation();
	const FVector StartLocation   = CharacterOwner->BulletTraceRelativeLocation();
	const FVector EndLocation     = StartLocation + (ForwardLocation * ForwardOffset);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = FName("");
	CollisionQueryParams.OwnerTag = FName("");
	CollisionQueryParams.bTraceAsyncScene = false;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	CollisionQueryParams.AddIgnoredActor(this);

	bool bSuccess = World->LineTraceSingleByChannel(
		HitData, 
		StartLocation, 
		EndLocation, 
		ECollisionChannel::ECC_Camera, 
		CollisionQueryParams);

	// temp draw
	DrawDebugLine(World, HitData.TraceStart, HitData.TraceEnd, FColor(255, 0, 0), false, -1, 0, 12.333);

	const FVector MuzzleLocation  = GetMuzzleTransform().GetLocation();
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	UGameplayStatics::PlaySoundAtLocation(World, FireSoundAsset, MuzzleLocation, 1.f, 1.f, 0.f, nullptr, nullptr);
	CharacterOwner->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	UGameplayStatics::PlaySoundAtLocation(World, FireImpactSoundAsset, HitData.Location, 1.f, 1.f, 0.f, nullptr, nullptr);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint   = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation  = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = World->SpawnActor<ABulletBase>(BulletsBP, Transform, SpawnParams);

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
				auto d = WeaponItemInfo.Damage;
				auto dHalf = d * 0.5f;
				float Damage = FMath::FRandRange(dHalf, d);
				CombatInterface->OnTakeDamage_Implementation(
					HitData.BoneName, 
					Damage, 
					CharacterOwner);
			}
		}
	}

	// spawn impact emitter
	FTransform EmitterTransform;
	EmitterTransform.SetIdentity();
	EmitterTransform.SetLocation(HitData.Location);
	UParticleSystemComponent* ImpactMetalEmitterComponent = UGameplayStatics::SpawnEmitterAtLocation(
		World, 
		ImpactMetalEmitterTemplate, 
		EmitterTransform, 
		true);

	// attach muzzleflash emitter
	UParticleSystemComponent* MuzzleFlashEmitterComponent = UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlashEmitterTemplate, 
		GetSkeletalMeshComponent(),
		MuzzleSocketName, 
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
		bEmpty = true;
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
	World->GetTimerManager().SetTimer(ReloadTimerHandle, TimerCallback, ReloadDuration, false);
}

void AWeaponBase::OnFireReleaseInternal()
{
}

void AWeaponBase::OnReloadInternal()
{
	bFired = false;
	CharacterOwner->ReloadActionMontage();
	NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);

	if (WeaponItemInfo.MaxAmmo <= NeededAmmo)
	{
		WeaponItemInfo.MaxAmmo = 0;
		WeaponItemInfo.CurrentAmmo = (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo);
	}
	else
	{
		WeaponItemInfo.MaxAmmo = (WeaponItemInfo.MaxAmmo - NeededAmmo);
		WeaponItemInfo.CurrentAmmo = WeaponItemInfo.ClipType;
	}
}

void AWeaponBase::SetCharacterOwner(ACharacterBase* InCharacterOwner)
{
	CharacterOwner = InCharacterOwner;
	OwnerClass = InCharacterOwner ? InCharacterOwner->StaticClass() : nullptr;
}

void AWeaponBase::CopyTo(const FWeaponItemInfo & InWeaponItemInfo)
{
	WeaponItemInfo.CopyTo(InWeaponItemInfo);
}
