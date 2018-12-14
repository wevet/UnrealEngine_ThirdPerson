// Copyright 2018 wevet works All Rights Reserved.

#include "WeaponBase.h"
#include "CharacterBase.h"
#include "BulletBase.h"
#include "CharacterPickupComponent.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AWeaponBase::AWeaponBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	CharacterOwner(nullptr), 
	WidgetComponent(nullptr),
	SphereComponent(nullptr),
	SkeletalMeshComponent(nullptr),
	MuzzleSocketName(FName(TEXT("MuzzleFlash"))),
	BulletDuration(0.1f),
	ReloadDuration(2.f),
	bEmpty(false),
	bEquip(false),
	bReload(false),
	bFired(false)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent  = SceneComponent;

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(SkeletalMeshComponent);
	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(180.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(SkeletalMeshComponent);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(WidgetComponent && WidgetComponent->IsValidLowLevel())) 
	{
		WidgetComponent->SetVisibility(false);
	}

	if (ensure(SphereComponent && SphereComponent->IsValidLowLevel())) 
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
	UWorld* const World = GetWorld();
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
		if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
		{
			SetCharacterOwner(Character);
		}
	}

	WidgetComponent->SetVisibility(true);
	SkeletalMeshComponent->SetRenderCustomDepth(true);
	if (CharacterOwner)
	{
		CharacterOwner->GetPickupComponent()->SetPickupActor(this);
	}
}

void AWeaponBase::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CharacterOwner == nullptr)
	{
		if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
		{
			SetCharacterOwner(Character);
		}
	}

	WidgetComponent->SetVisibility(false);
	SkeletalMeshComponent->SetRenderCustomDepth(false);
	if (CharacterOwner)
	{
		CharacterOwner->GetPickupComponent()->SetPickupActor(nullptr);
	}
}

void AWeaponBase::OnFirePressedInternal()
{
	UWorld* const World = GetWorld();

	// not found owner
	if (World == nullptr || CharacterOwner == nullptr || (CharacterOwner && CharacterOwner->IsDeath_Implementation()))
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

	const bool bSuccess = World->LineTraceSingleByChannel(
		HitData, 
		StartLocation, 
		EndLocation, 
		ECollisionChannel::ECC_Camera, 
		CollisionQueryParams);

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

	TakeHitDamage(HitData);

	FTransform EmitterTransform;
	EmitterTransform.SetIdentity();
	EmitterTransform.SetLocation(HitData.Location);
	UParticleSystemComponent* ImpactMetalEmitterComponent = UGameplayStatics::SpawnEmitterAtLocation(
		World, 
		ImpactMetalEmitterTemplate, 
		EmitterTransform, 
		true);

	UParticleSystemComponent* MuzzleFlashEmitterComponent = UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlashEmitterTemplate, 
		GetSkeletalMeshComponent(),
		MuzzleSocketName, 
		MuzzleLocation,
		MuzzleRotation,
		EAttachLocation::KeepWorldPosition, 
		true);
}

void AWeaponBase::TakeHitDamage(const FHitResult HitResult)
{
	if (!ensure(HitResult.Actor.IsValid()))
	{
		return;
	}

	if (ICombatExecuter* CombatInterface = Cast<ICombatExecuter>(HitResult.Actor))
	{
		if (!CombatInterface->IsDeath_Implementation())
		{
			auto d = WeaponItemInfo.Damage;
			auto dHalf = d * 0.05f;
			float Damage = FMath::FRandRange(dHalf, d);
			CombatInterface->OnTakeDamage_Implementation(HitResult.BoneName, Damage, CharacterOwner);
		}
	}
}

void AWeaponBase::OnReloading_Implementation()
{
	UWorld* const World = GetWorld();
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

void AWeaponBase::SetCharacterOwner(ACharacterBase* NewCharacter)
{
	CharacterOwner = NewCharacter;
	//auto A = &NewCharacter;
	//auto B = (*A);
	//(*B).Equipment();
	//(*A)->Equipment();
}

void AWeaponBase::CopyTo(const FWeaponItemInfo& InWeaponItemInfo)
{
	//WeaponItemInfo = InWeaponItemInfo;
	//InWeaponItemInfo << WeaponItemInfo;
	//WeaponItemInfo.CopyTo(InWeaponItemInfo);
	//CloneTo(WeaponItemInfo, InWeaponItemInfo);
	//UE_LOG(LogWevetClient, Log, TEXT("WeaponCopy : %s \n MaxAmmo : %d"), *GetName(), WeaponItemInfo.MaxAmmo);
}

// @TODO
void AWeaponBase::CloneTo(FWeaponItemInfo& OutWeaponItemInfo, const FWeaponItemInfo& InWeaponItemInfo)
{
	OutWeaponItemInfo.UnEquipSocketName = InWeaponItemInfo.UnEquipSocketName;
	OutWeaponItemInfo.EquipSocketName   = InWeaponItemInfo.EquipSocketName;
	OutWeaponItemInfo.WeaponItemType    = InWeaponItemInfo.WeaponItemType;
	OutWeaponItemInfo.WeaponClass = InWeaponItemInfo.WeaponClass;
	OutWeaponItemInfo.CurrentAmmo = InWeaponItemInfo.CurrentAmmo;
	OutWeaponItemInfo.ClipType = InWeaponItemInfo.ClipType;
	OutWeaponItemInfo.MaxAmmo  = InWeaponItemInfo.MaxAmmo;
	OutWeaponItemInfo.Damage   = InWeaponItemInfo.Damage;
	OutWeaponItemInfo.Texture  = InWeaponItemInfo.Texture;
}
