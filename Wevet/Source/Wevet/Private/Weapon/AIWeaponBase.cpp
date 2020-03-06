// Copyright © 2018 wevet works All Rights Reserved.

#include "Weapon/AIWeaponBase.h"
#include "Character/CharacterBase.h"
#include "AI/AICharacterBase.h"
#include "Weapon/BulletBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAIWeaponBase::AAIWeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAIWeaponBase::OnFirePressInternal()
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (!CanMeleeStrike_Implementation())
	{
		return;
	}

	if (Super::bReload)
	{
		return;
	}

	if (Super::WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Out Of Ammos"));
		Super::DoReload_Implementation();
		return;
	}

	const FVector StartLocation   = CharacterPtr.Get()->BulletTraceRelativeLocation();
	const FVector ForwardLocation = CharacterPtr.Get()->BulletTraceForwardLocation();
	const FVector EndLocation = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = FName("");
	CollisionQueryParams.OwnerTag = FName("");
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	CollisionQueryParams.AddIgnoredActors(IgnoreActors);

	const bool bSuccess = World->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionQueryParams);

	const FVector MuzzleLocation  = StartLocation;
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	IInteractionExecuter::Execute_ReportNoiseOther(CharacterPtr.Get(), this, FireSound, DEFAULT_VOLUME, MuzzleLocation);
	CharacterPtr.Get()->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	IInteractionExecuter::Execute_ReportNoiseOther(CharacterPtr.Get(), this, ImpactSound, DEFAULT_VOLUME, HitData.Location);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint   = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation  = UKismetMathLibrary::FindLookAtRotation(StartPoint, HitData.ImpactPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = World->SpawnActor<ABulletBase>(Super::BulletsTemplate, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	bool bCanDamageResult = false;
	Super::CheckIfValid(HitData.GetActor(), bCanDamageResult);
	if (bCanDamageResult)
	{
		Super::TakeDamageOuter(HitData.GetActor(), HitData.BoneName, WeaponItemInfo.Damage);
	}
	Super::PlayEffect(HitData, SkeletalMeshComponent);
}
