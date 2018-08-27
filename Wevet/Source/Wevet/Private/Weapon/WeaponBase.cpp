// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "MockCharacter.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

AWeaponBase::AWeaponBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	CharacterOwner(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	this->MuzzleSocketName = FName(TEXT("MuzzleFlash"));
	this->BulletDuration = 0.1F;

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

void AWeaponBase::OnFirePress_Implementation()
{
	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		if (this->CanFired)
		{
			this->CanFired = false;
		}
		UE_LOG(LogTemp, Log, TEXT("Out Of Ammos"));
		return;
	}
	this->CanFired = false;

}

void AWeaponBase::OnFireRelease_Implementation()
{
	this->CanFired = false;
}

void AWeaponBase::OnReloading_Implementation()
{
	// reloading event
}

void AWeaponBase::OffVisible_Implementation()
{
	this->Visible = false;
	this->SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->SkeletalMeshComponent->SetSimulatePhysics(false);
	this->WidgetComponent->SetVisibility(this->Visible);
	GLog->Log("Off Visible " + Super::GetName());
}

void AWeaponBase::OnVisible_Implementation()
{
	this->Visible = true;
	this->SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->SkeletalMeshComponent->SetSimulatePhysics(true);
	this->WidgetComponent->SetVisibility(this->Visible);
	GLog->Log("On Visible " + Super::GetName());
}

void AWeaponBase::BeginOverlapRecieve(
	UPrimitiveComponent * OverlappedComponent, 
	AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult & SweepResult)
{
	if (this->CharacterOwner == nullptr)
	{
		AMockCharacter* MockCharacter = Cast<AMockCharacter>(OtherActor);

		if (MockCharacter)
		{
			this->CharacterOwner = MockCharacter;
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

void AWeaponBase::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (this->CharacterOwner == nullptr)
	{
		AMockCharacter* MockCharacter = Cast<AMockCharacter>(OtherActor);

		if (MockCharacter)
		{
			this->CharacterOwner = MockCharacter;
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

