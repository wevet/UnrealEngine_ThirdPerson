// Copyright 2018 wevet works All Rights Reserved.

#include "Weapon/Rifle.h"
#include "WevetExtension.h"

ARifle::ARifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BulletDuration = 0.15f;
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent->SetBoxExtent(FVector(16.f, 80.f, 32.f));

	MeleeCollisionComponent->SetRelativeLocation(FVector(0.0f, 24.f, 18.f));
	MeleeCollisionComponent->SetBoxExtent(FVector(3.f, 28.f, 12.f));

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
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_AssaultRifle_MuzzleFlash"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Medium_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetRifleBulletPath());
		BulletsTemplate = FindAsset.Object;
	}

	// AK47
	WeaponItemInfo.NeededAmmo = 30;
	WeaponItemInfo.MaxAmmo = 600;
	WeaponItemInfo.EquipSocketName = FName(TEXT("RifleEquip_Socket"));
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Rifle_Socket"));
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Rifle;
	WeaponItemInfo.DisplayName = FString(TEXT("FN SCAR"));
	WeaponItemInfo.MeleeDistance = 2000.f;
	WeaponItemInfo.HearingRange = 1400.f;
}

