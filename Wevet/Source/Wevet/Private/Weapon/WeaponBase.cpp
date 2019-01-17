// Copyright 2018 wevet works All Rights Reserved.

#include "WeaponBase.h"
#include "CharacterBase.h"
#include "BulletBase.h"
#include "CharacterPickupComponent.h"
#include "CharacterModel.h"
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
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent  = SceneComponent;

	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(RootComponent);

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SphereComponent);
	SkeletalMeshComponent->bRenderCustomDepth = true;
	SkeletalMeshComponent->CustomDepthStencilValue = 1;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(180.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(SphereComponent);
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
	SphereComponent->Deactivate();
}

void AWeaponBase::OnVisible_Implementation()
{
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SkeletalMeshComponent->SetSimulatePhysics(true);
	WidgetComponent->SetVisibility(true);
	SphereComponent->Activate();
}

void AWeaponBase::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult & SweepResult)
{
	WidgetComponent->SetVisibility(true);
	SkeletalMeshComponent->SetRenderCustomDepth(true);

	if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
	{
		Character->GetPickupComponent()->SetPickupActor(this);
	}
}

void AWeaponBase::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	WidgetComponent->SetVisibility(false);
	SkeletalMeshComponent->SetRenderCustomDepth(false);
	if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
	{
		Character->GetPickupComponent()->SetPickupActor(nullptr);
	}
}

void AWeaponBase::OnFirePressedInternal()
{
	UWorld* const World = GetWorld();

	// not found owner
	if (World == nullptr || CharacterOwner == nullptr || (CharacterOwner && ICombatExecuter::Execute_IsDeath(CharacterOwner)))
	{
		return;
	}

	if (bEmpty || bReload)
	{
		return;
	}

	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Out Of Ammos"));
		OnReloading_Implementation();
		return;
	}

	const FVector StartLocation   = CharacterOwner->BulletTraceRelativeLocation();
	const FVector ForwardLocation = CharacterOwner->BulletTraceForwardLocation();
	const FVector EndLocation     = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

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

	//World->AsyncLineTraceByChannel(StartLocation, EndLocation, ECollisionChannel::ECC_Camera, CollisionQueryParams);

	const FVector MuzzleLocation  = GetMuzzleTransform().GetLocation();
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	const float Volume = 1.0f;

	IInteractionExecuter::Execute_ReportNoiseOther(CharacterOwner, this, FireSoundAsset, Volume, MuzzleLocation);
	CharacterOwner->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	IInteractionExecuter::Execute_ReportNoiseOther(CharacterOwner, this, FireImpactSoundAsset, Volume, HitData.Location);
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
	PlayBulletEffect(World, HitData);
}

void AWeaponBase::PlayBulletEffect(UWorld* const World, const FHitResult HitResult)
{
	FTransform EmitterTransform;
	EmitterTransform.SetIdentity();
	EmitterTransform.SetLocation(HitResult.Location);

	UParticleSystemComponent* ImpactMetalEmitterComponent = UGameplayStatics::SpawnEmitterAtLocation(
		World,
		ImpactMetalEmitterTemplate,
		EmitterTransform,
		true);

	UParticleSystemComponent* MuzzleFlashEmitterComponent = UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlashEmitterTemplate,
		GetSkeletalMeshComponent(),
		MuzzleSocketName,
		GetMuzzleTransform().GetLocation(),
		FRotator(GetMuzzleTransform().GetRotation()),
		EAttachLocation::KeepWorldPosition,
		true);
}

void AWeaponBase::TakeHitDamage(const FHitResult HitResult)
{
	if (!HitResult.Actor.IsValid())
	{
		return;
	}

	ICombatExecuter* CombatExecuter = Cast<ICombatExecuter>(HitResult.GetActor());
	if (CombatExecuter == nullptr)
	{
		return;
	}
	if (CombatExecuter->Execute_IsDeath(HitResult.GetActor()))
	{
		return;
	}

	const float Offset = 0.05f;
	const int32 Attack = CharacterOwner->GetCharacterModel()->GetAttack();
	const int32 Wisdom = CharacterOwner->GetCharacterModel()->GetWisdom();
	const float WeaponDamage = WeaponItemInfo.Damage;
	const float Total = (float)(Attack / Wisdom) + WeaponDamage;
	const float Damage = FMath::FRandRange((Total * Offset), Total);
	CombatExecuter->Execute_OnTakeDamage(HitResult.GetActor(), HitResult.BoneName, Damage, CharacterOwner);
}

void AWeaponBase::OnReloading_Implementation()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (WeaponItemInfo.MaxAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Empty Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		bEmpty = true;
		return;
	}

	if (WeaponItemInfo.CurrentAmmo >= WeaponItemInfo.ClipType)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Full Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
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

void AWeaponBase::Take(ACharacterBase* NewCharacter)
{
	SetCharacterOwner(NewCharacter);
	SetEquip(false);
	OffVisible_Implementation();

	if (ensure(SphereComponent && SphereComponent->IsValidLowLevel()))
	{
		SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AWeaponBase::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &AWeaponBase::EndOverlapRecieve);
	}
	check(GetWorld());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSoundAsset, CharacterOwner ? CharacterOwner->GetActorLocation() : GetActorLocation());
}

void AWeaponBase::Release(ACharacterBase* NewCharacter)
{
	SetCharacterOwner(NewCharacter);
	SetEquip(false);
	//OnVisible_Implementation();
	//if (ensure(SphereComponent && SphereComponent->IsValidLowLevel()))
	//{
	//	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::BeginOverlapRecieve);
	//	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::EndOverlapRecieve);
	//}
	if (IsValidLowLevel())
	{
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}
}

void AWeaponBase::Recover(const FWeaponItemInfo RefWeaponItemInfo)
{
	WeaponItemInfo.MaxAmmo += RefWeaponItemInfo.MaxAmmo;
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
	SetOwner(CharacterOwner);
	//auto A = &NewCharacter;
	//auto B = (*A);
	//(*B).Equipment();
	//(*A)->Equipment();
}

void AWeaponBase::CopyWeaponItemInfo(const FWeaponItemInfo RefWeaponItemInfo)
{
	WeaponItemInfo.UnEquipSocketName = RefWeaponItemInfo.UnEquipSocketName;
	WeaponItemInfo.EquipSocketName = RefWeaponItemInfo.EquipSocketName;
	WeaponItemInfo.WeaponItemType  = RefWeaponItemInfo.WeaponItemType;
	WeaponItemInfo.WeaponClass = RefWeaponItemInfo.WeaponClass;
	WeaponItemInfo.CurrentAmmo = RefWeaponItemInfo.CurrentAmmo;
	WeaponItemInfo.ClipType = RefWeaponItemInfo.ClipType;
	WeaponItemInfo.MaxAmmo  = RefWeaponItemInfo.MaxAmmo;
	WeaponItemInfo.Damage   = RefWeaponItemInfo.Damage;
	WeaponItemInfo.Texture  = RefWeaponItemInfo.Texture;
}
