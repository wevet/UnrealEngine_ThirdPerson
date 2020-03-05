// Copyright 2018 wevet works All Rights Reserved.

#include "WeaponBase.h"
#include "CharacterBase.h"
#include "BulletBase.h"
#include "CharacterPickupComponent.h"
#include "CharacterModel.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"

using namespace Wevet;

AWeaponBase::AWeaponBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	MuzzleSocketName(FName(TEXT("MuzzleFlash"))),
	BulletDuration(0.1f),
	ReloadDuration(2.f)
{
	bEmpty  = false;
	bEquip  = false;
	bReload = false;
	bFired  = false;
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent  = SceneComponent;

	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(RootComponent);

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SphereComponent);
	SkeletalMeshComponent->bRenderCustomDepth = false;
	SkeletalMeshComponent->CustomDepthStencilValue = 1;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(180.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(SphereComponent);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (ComponentExtension::HasValid(WidgetComponent))
	{
		WidgetComponent->SetVisibility(false);
	}

	if (ComponentExtension::HasValid(SphereComponent))
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
	PrepareDestroy();
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
	if (World == nullptr || 
		!CharacterOwner.IsValid() || 
		IDamageInstigator::Execute_IsDeath(CharacterOwner.Get()))
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
	//CollisionQueryParams.bTraceAsyncScene = false;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(CharacterOwner.Get());

	const bool bSuccess = World->LineTraceSingleByChannel(
		HitData, 
		StartLocation, 
		EndLocation, 
		ECollisionChannel::ECC_Camera, 
		CollisionQueryParams);

	const FVector MuzzleLocation  = GetMuzzleTransform().GetLocation();
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	IInteractionExecuter::Execute_ReportNoiseOther(CharacterOwner.Get(), this, FireSoundAsset, DEFAULT_VOLUME, MuzzleLocation);
	CharacterOwner->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	IInteractionExecuter::Execute_ReportNoiseOther(CharacterOwner.Get(), this, FireImpactSoundAsset, DEFAULT_VOLUME, HitData.Location);
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

	UGameplayStatics::SpawnEmitterAtLocation(
		World,
		ImpactMetalEmitterTemplate,
		EmitterTransform,
		true);

	UGameplayStatics::SpawnEmitterAttached(
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

	AActor* HitActor = HitResult.GetActor();
	ACharacterBase* Target = Cast<ACharacterBase>(HitActor);

	if (Target == nullptr)
	{
		return;
	}

	auto Character = CharacterOwner.Get();
	const float WeaponDamage = WeaponItemInfo.Damage;
	UCharacterModel* const Model = IDamageInstigator::Execute_GetPropertyModel(Character);
	UCharacterModel* const TargetModel = IDamageInstigator::Execute_GetPropertyModel(Target);
	if (Model && TargetModel)
	{
		const int32 BaseAttack = Model->GetAttack() + WeaponDamage;
		const int32 Attack = BaseAttack;
		const int32 Deffence = TargetModel->GetDefence();
		const int32 Wisdom = TargetModel->GetWisdom();
		const int32 TotalDamage = (Attack - (Deffence + Wisdom)) / DEFFENCE_CONST;

		float Damage = (float)TotalDamage;
		Damage = FMath::Abs(Damage);

		bool bDie = false;
		IDamageInstigator::Execute_OnTakeDamage(Target, HitResult.BoneName, Damage, Character, bDie);
		UE_LOG(LogWevetClient, Log, TEXT("Damage : %f \n funcName : %s"), Damage, *FString(__FUNCTION__));
	}
	else
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr Interface \n funcName : %s"), *FString(__FUNCTION__));
	}
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
	TimerCallback.BindLambda([&]
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

	if (ComponentExtension::HasValid(SphereComponent))
	{
		SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AWeaponBase::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &AWeaponBase::EndOverlapRecieve);
	}

	UWorld* const World = GetWorld();

	if (PickupSoundAsset && World)
	{
		const FVector Location = CharacterOwner.IsValid() ? CharacterOwner.Get()->GetActorLocation() : GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(World, PickupSoundAsset, Location);
	}
	SkeletalMeshComponent->SetRenderCustomDepth(false);
}

void AWeaponBase::Release(ACharacterBase* NewCharacter)
{
	SetCharacterOwner(NewCharacter);
	SetEquip(false);

	if (IsValidLowLevel())
	{
		PrepareDestroy();
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
	CharacterOwner = MakeWeakObjectPtr<ACharacterBase>(NewCharacter);
	Super::SetOwner(CharacterOwner.IsValid() ? CharacterOwner.Get() : nullptr);
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

void AWeaponBase::PrepareDestroy()
{
	UWorld* const World = GetWorld();
	if (World && World->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
	if (CharacterOwner.IsValid())
	{
		CharacterOwner.Reset();
	}
}
