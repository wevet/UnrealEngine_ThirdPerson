// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/Rifle.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Weapon/BulletBase.h"
#include "WevetExtension.h"
#include "Player/MockCharacter.h"


ARifle::ARifle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(RootComponent);

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SphereComponent);
	SkeletalMeshComponent->bRenderCustomDepth = false;
	SkeletalMeshComponent->CustomDepthStencilValue = 1;

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Weapons/Assault_Rifle_A"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Rifle/Cues/RifleA_Fire_Cue"));
		FireSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Rifle/Cues/Rifle_ImpactSurface_Cue"));
		ImpactSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Engine/VREditor/Sounds/UI/Object_PickUp"));
		PickupSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_AssaultRifle_MuzzleFlash"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Medium_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Bullet/BP_MasterBullet.BP_MasterBullet_C"));
		BulletsTemplate = FindAsset.Object;
	}
}

void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARifle::BeginPlay()
{
	Super::BeginPlay();

	if (Wevet::ComponentExtension::HasValid(SphereComponent))
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ARifle::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ARifle::EndOverlapRecieve);
	}
}

void ARifle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool ARifle::CanMeleeStrike_Implementation() const
{
	// was nullptr
	if (!Super::CharacterPtr.IsValid())
	{
		return false;
	}

	// was death
	if (IDamageInstigator::Execute_IsDeath(CharacterPtr.Get()))
	{
		return false;
	}
	bool bWasEmpty = (WeaponItemInfo.MaxAmmo <= 0);
	return (bWasEmpty == false);
}

void ARifle::DoReload_Implementation()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (!CanMeleeStrike_Implementation())
	{
		UE_LOG(LogWevetClient, Log, TEXT("Empty Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	if (WeaponItemInfo.CurrentAmmo >= WeaponItemInfo.ClipType)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Full Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	SetReload(true);
	GetPointer()->ReloadActionMontage(ReloadDuration);
	OnReloadInternal();
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		SetReload(false);
	});
	World->GetTimerManager().SetTimer(ReloadTimerHandle, TimerCallback, ReloadDuration, false);
}

void ARifle::OnReloadInternal()
{
	bFirePressed = false;
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

void ARifle::OnFirePressInternal()
{
	if (!CanMeleeStrike_Implementation())
	{
		return;
	}

	if (bReload)
	{
		return;
	}

	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Out Of Ammos : %s"), *FString(__FUNCTION__));
		DoReload_Implementation();
		return;
	}

	const FVector StartLocation = GetPointer()->BulletTraceRelativeLocation();
	const FVector ForwardLocation = GetPointer()->BulletTraceForwardLocation();
	const FVector EndLocation = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

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
	CollisionQueryParams.AddIgnoredActors(IgnoreActors);

	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Camera,
		CollisionQueryParams);

	const FTransform Trans = GetMuzzleTransform();
	const FVector MuzzleLocation  = Trans.GetLocation();
	const FRotator MuzzleRotation = FRotator(Trans.GetRotation());
	IInteractionExecuter::Execute_ReportNoiseOther(GetPointer(), this, FireSound, DEFAULT_VOLUME, MuzzleLocation);
	GetPointer()->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	IInteractionExecuter::Execute_ReportNoiseOther(GetPointer(), this, ImpactSound, DEFAULT_VOLUME, HitData.Location);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint  = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletsTemplate, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	// Effect
	Super::PlayEffect(HitData, SkeletalMeshComponent);

	bool bCanDamageResult = false;
	Super::CheckIfValid(HitData.Actor.Get(), bCanDamageResult);
	if (bCanDamageResult)
	{
		//Super::TakeDamageOuter(HitData.Actor.Get(), Hid)
	}
}

void ARifle::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (GetOwner())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Already Owner : %s"), *GetOwner()->GetName());
		return;
	}
	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherActor))
	{
		SkeletalMeshComponent->SetRenderCustomDepth(true);
		Character->GetPickupComponent()->SetPickupActor(this);
	}
}

void ARifle::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (GetOwner())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Already Owner : %s"), *GetOwner()->GetName());
		return;
	}
	if (AMockCharacter * Character = Cast<AMockCharacter>(OtherActor))
	{
		SkeletalMeshComponent->SetRenderCustomDepth(false);
		Character->GetPickupComponent()->SetPickupActor(nullptr);
	}
}

FTransform ARifle::GetMuzzleTransform() const
{
	return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName);
}
