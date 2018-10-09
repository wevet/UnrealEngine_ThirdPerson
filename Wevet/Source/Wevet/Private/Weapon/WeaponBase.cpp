// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "MockCharacter.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponBase::AWeaponBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	CharacterOwner(nullptr), 
	WidgetComponent(nullptr),
	SphereComponent(nullptr),
	SkeletalMeshComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	this->MuzzleSocketName = FName(TEXT("MuzzleFlash"));
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

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(180.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(SkeletalMeshComponent);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	if (WidgetComponent) 
	{
		WidgetComponent->SetVisibility(this->Visible);
	}
	if (SphereComponent && SphereComponent->IsValidLowLevel())
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::EndOverlapRecieve);
	}
}

void AWeaponBase::BulletFireCoolDown()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnCoolDownCheckDelay FIRED!!! ... "));
	FLatentActionInfo ActionInfo;
	ActionInfo.Linkage = 0;
	ActionInfo.CallbackTarget = this;
	ActionInfo.ExecutionFunction = "BulletFireCoolDownTimer";
	ActionInfo.UUID = 53344322;
	UKismetSystemLibrary::RetriggerableDelay(World, this->ReloadDuration, ActionInfo);
}

void AWeaponBase::BulletFireCoolDownTimer()
{
	this->bBulletFireCoolDownSuccess = true;
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::SetFireSoundAsset(USoundBase * FireSoundAsset)
{
	this->FireSoundAsset = FireSoundAsset;
}

void AWeaponBase::SetFireAnimMontageAsset(UAnimMontage * FireAnimMontageAsset)
{
	this->FireAnimMontageAsset = FireAnimMontageAsset;
}

void AWeaponBase::SetReloadAnimMontageAsset(UAnimMontage * ReloadAnimMontageAsset)
{
	this->ReloadAnimMontageAsset = ReloadAnimMontageAsset;
}

void AWeaponBase::OnEquip(bool Equip)
{
	this->Equip = Equip;
}

void AWeaponBase::SetPickable(bool Pick)
{
	this->CanPick = Pick;
}

void AWeaponBase::SetReload(bool Reload)
{
	this->IsReload = Reload;
}

// fire pressed
void AWeaponBase::OnFirePress_Implementation()
{
	this->CanFired = true;
}

// fire released
void AWeaponBase::OnFireRelease_Implementation()
{
	this->CanFired = false;

	// empty clip size
	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Out Of Ammos"));
		OnReloading_Implementation();
	}
}

void AWeaponBase::OnReloading_Implementation()
{
	if (WeaponItemInfo.MaxAmmo <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Empty Ammos"));
		return;
	}

	if (WeaponItemInfo.ClipType >= WeaponItemInfo.CurrentAmmo)
	{
		UE_LOG(LogTemp, Log, TEXT("Full Ammos"));
		return;
	}
	SetReload(true);

}

void AWeaponBase::OffVisible_Implementation()
{
	this->Visible = false;
	this->SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->SkeletalMeshComponent->SetSimulatePhysics(false);
	this->WidgetComponent->SetVisibility(this->Visible);
}

void AWeaponBase::OnVisible_Implementation()
{
	this->Visible = true;
	this->SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->SkeletalMeshComponent->SetSimulatePhysics(true);
	this->WidgetComponent->SetVisibility(this->Visible);
}

void AWeaponBase::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (this->CharacterOwner == nullptr)
	{
		AMockCharacter* MockCharacter = Cast<AMockCharacter>(OtherActor);
		if (MockCharacter)
		{
			this->CharacterOwner = MockCharacter;
			check(this->CharacterOwner != nullptr);
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
		AMockCharacter* MockCharacter = Cast<AMockCharacter>(OtherActor);
		if (MockCharacter)
		{
			this->CharacterOwner = MockCharacter;
			check(this->CharacterOwner != nullptr);
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

void AWeaponBase::OnFirePressedInternal(const FVector RelativeLocation, float ForwardOffset = 15000.f)
{
	UWorld* World = GetWorld();

	// not found owner
	if (this->CharacterOwner == nullptr || World == nullptr)
	{
		return;
	}

	// current weapon reloading...
	if (this->IsReload)
	{
		UE_LOG(LogTemp, Log, TEXT("Now Reloading..."));
		return;
	}

	// empty clip size
	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		if (this->CanFired)
		{
			this->CanFired = false;
		}
		UE_LOG(LogTemp, Log, TEXT("Out Of Ammos"));
		return;
	}

	const FVector StartLocation = RelativeLocation;
	const FVector EndLocation = StartLocation + (this->CharacterOwner->GetFollowCameraComponent()->GetForwardVector() * ForwardOffset);

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

	UGameplayStatics::PlaySoundAtLocation(World, GetFireSoundAsset(), MuzzleLocation, 1.f, 1.f, 0.f, nullptr, nullptr);
	this->CharacterOwner->PlayAnimMontage(GetFireAnimMontageAsset(), 1.0f);
	--WeaponItemInfo.CurrentAmmo;

	UGameplayStatics::PlaySoundAtLocation(World, GetFireImpactSoundAsset(), HitData.Location, 1.f, 1.f, 0.f, nullptr, nullptr);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint   = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation  = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	const ABulletBase* Bullet = World->SpawnActor<ABulletBase>(this->BulletsBP, Transform, SpawnParams);

	if (HitData.Actor != nullptr)
	{
		ICombat* CombatInterface = Cast<ICombat>(HitData.Actor);
		if (bSuccess && CombatInterface)
		{
			float Damage = (FMath::FRandRange(20.f, 35.f) / 1000.f);
			CombatInterface->OnTakeDamage_Implementation(HitData.BoneName, Damage, this);
		}
	}

	// spawn impact emitter
	const FTransform EmitterTransform = UKismetMathLibrary::MakeTransform(HitData.Location, FRotator::ZeroRotator, FVector::ZeroVector);
	UParticleSystemComponent* ImpactMetalEmitterComponent = UGameplayStatics::SpawnEmitterAtLocation(World, this->ImpactMetalEmitterTemplate, EmitterTransform, true);

	// attach muzzleflash emitter
	UParticleSystemComponent* MuzzleFlashEmitterComponent = UGameplayStatics::SpawnEmitterAttached(
		this->MuzzleFlashEmitterTemplate, 
		GetSkeletalMeshComponent(),
		GetMuzzleSocket(), 
		MuzzleLocation,
		MuzzleRotation,
		EAttachLocation::KeepWorldPosition, 
		true);
}

void AWeaponBase::OnFireReleaseInternal()
{
}

