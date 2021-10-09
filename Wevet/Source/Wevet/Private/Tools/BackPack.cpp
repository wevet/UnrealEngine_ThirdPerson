// Copyright 2018 wevet works All Rights Reserved.


#include "Tools/BackPack.h"
#include "Character/CharacterBase.h"
#include "Item/AbstractWeapon.h"


ABackPack::ABackPack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	RootComponent = SkeletalMeshComponent;
	SkeletalMeshComponent->PhysicsTransformUpdateMode = EPhysicsTransformUpdateMode::SimulationUpatesComponentTransform;
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/Backpack/Meshs/SM_BackPack"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	BoxCollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxCollisionComponent"));
	BoxCollisionComponent->SetupAttachment(SkeletalMeshComponent);
	BoxCollisionComponent->SetGenerateOverlapEvents(false);
	BoxCollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCollisionComponent->SetRelativeLocation(FVector(0.0f, -30.f, 66.f));
	BoxCollisionComponent->SetBoxExtent(FVector(15.f, 14.f, 24.f));
}


void ABackPack::BeginPlay()
{
	Super::BeginPlay();
	StopSimulatePhysics();
}


void ABackPack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopSimulatePhysics();
}


void ABackPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABackPack::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	check(SkeletalMeshComponent);
	SkeletalMeshComponent->SetOwnerNoSee(NewOwnerNoSee);
}


void ABackPack::StoreWeapon(AAbstractWeapon* InWeapon, bool& OutAttachSuccess)
{
	if (InWeapon == nullptr)
	{
		return;
	}

	if (InWeapon->GetWeaponItemType() == EWeaponItemType::None)
	{
		UE_LOG(LogWevetClient, Error, TEXT("Unknown item type. : %s"), *InWeapon->GetName());
		return;
	}

	if (InWeapon->GetWeaponItemType() == EWeaponItemType::Naked ||
		InWeapon->GetWeaponItemType() == EWeaponItemType::Knife)
	{
		UE_LOG(LogWevetClient, Warning, TEXT("These can not be stored. : %s"), *InWeapon->GetName());
		return;
	}

	const FName SocketName(InWeapon->GetWeaponItemInfo().UnEquipSocketName);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	InWeapon->AttachToComponent(SkeletalMeshComponent, Rules, SocketName);
	InWeapon->SetEquip(false);
	OutAttachSuccess = true;
}


void ABackPack::StartSimulatePhysics()
{
	SkeletalMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	BP_OnSimulatePhysics(false);
}


void ABackPack::StopSimulatePhysics()
{
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	const FTransform Trans = FTransform::Identity;
	SkeletalMeshComponent->SetRelativeTransform(Trans);
	BP_OnSimulatePhysics(true);
}

