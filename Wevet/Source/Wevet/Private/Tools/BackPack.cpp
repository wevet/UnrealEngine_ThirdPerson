// Copyright 2018 wevet works All Rights Reserved.


#include "Tools/BackPack.h"
#include "Character/CharacterBase.h"
#include "Weapon/AbstractWeapon.h"

ABackPack::ABackPack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	RootComponent = SkeletalMeshComponent;
	SkeletalMeshComponent->PhysicsTransformUpdateMode = EPhysicsTransformUpdateMode::SimulationUpatesComponentTransform;
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/Backpack/Meshs/SM_BackPack"));
	USkeletalMesh* Asset = FindAsset.Object;
	SkeletalMeshComponent->SetSkeletalMesh(Asset);
}

void ABackPack::BeginPlay()
{
	Super::BeginPlay();
}

void ABackPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABackPack::SetOwnerNoSeeMesh(const bool NewOwnerNoSee)
{
	SkeletalMeshComponent->SetOwnerNoSee(NewOwnerNoSee);
}

void ABackPack::PutWeapon(AAbstractWeapon* InWeapon, bool& OutAttachSuccess)
{
	if (InWeapon == nullptr)
	{
		return;
	}

	if (InWeapon->GetWeaponItemType() == EWeaponItemType::None || InWeapon->GetWeaponItemType() == EWeaponItemType::Knife)
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Ignore Weapon : %s"), *InWeapon->GetName());
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
