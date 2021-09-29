// Copyright 2018 wevet works All Rights Reserved.


#include "Item/NakedWeapon.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Wevet.h"
#include "WevetExtension.h"
#include "Lib/CombatBlueprintFunctionLibrary.h"
#include "Perception/AISense_Hearing.h"


#define IMPACT_RANGE 400.f


ANakedWeapon::ANakedWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bRenderCutomDepthEnable = false;


	WeaponItemInfo.NeededAmmo = 0;
	WeaponItemInfo.ClipType = 0;
	WeaponItemInfo.CurrentAmmo = 0;
	WeaponItemInfo.MaxAmmo = 0;
	WeaponItemInfo.Damage = 500.f;
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Naked;
	WeaponItemInfo.MeleeDistance = 500.f;
	WeaponItemInfo.TraceDistance = 500.f;
	WeaponItemInfo.EquipSocketName = NAME_None;
	WeaponItemInfo.UnEquipSocketName = NAME_None;
	WeaponItemInfo.DisplayName = FString(TEXT("Bare Hands"));
	WeaponItemInfo.Priority = 0;

	MuzzleSocketName = NAME_None;
	GripSocketName = NAME_None;

	// Hidden Components
	SkeletalMeshComponent->SetVisibility(false);
	SkeletalMeshComponent->SetComponentTickEnabled(false);
	SkeletalMeshComponent->SetHiddenInGame(true);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	CollisionComponent->SetVisibility(false);
	CollisionComponent->SetComponentTickEnabled(false);
	CollisionComponent->SetHiddenInGame(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}


void ANakedWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	NakedTriggerArray.Reset(0);
	NakedWeaponTriggerMap.Reset();
}


void ANakedWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearNakedActionApply();
	RemoveDelegate();
	NakedTriggerArray.Reset(0);
	NakedWeaponTriggerMap.Reset();
	Super::EndPlay(EndPlayReason);
}


#pragma region Public
void ANakedWeapon::Initialize(APawn* const NewCharacterOwner)
{
	Super::Initialize(NewCharacterOwner);

	if (GetPawnOwner())
	{
		DoDeployTemplate();
	}
}
#pragma endregion


#pragma region Interface
void ANakedWeapon::DoFirePressed_Implementation()
{
	Super::DoFirePressed_Implementation();
	if (WeaponActionDelegate.IsBound())
	{
		WeaponActionDelegate.Broadcast(bCanFire);
	}
}


void ANakedWeapon::DoFireRelease_Implementation()
{
	Super::DoFireRelease_Implementation();
	if (WeaponActionDelegate.IsBound())
	{
		WeaponActionDelegate.Broadcast(bCanFire);
	}
}


bool ANakedWeapon::CanStrike_Implementation() const
{
	//unsafe pointer
	if (!CharacterPtr.IsValid())
	{
		return false;
	}

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(GetPawnOwner()))
	{
		if (Interface == nullptr)
		{
			return false;
		}

		if (ICombatInstigator::Execute_IsDeath(Interface->_getUObject()))
		{
			return false;
		}
	}

	if (!WasEquip())
	{
		UE_LOG(LogWevetClient, Log, TEXT("No Equip => %s"), *FString(__FUNCTION__));
		return false;
	}
	return true;
}
#pragma endregion


#pragma region Override
void ANakedWeapon::TakeDamageOuter(const FHitResult& HitResult)
{
	if (!HitResult.GetActor())
	{
		return;
	}

	// calcurate baseDamage
	float TotalDamage = UCombatBlueprintFunctionLibrary::CalcurateBaseDamage(
		HitResult,
		this,
		GetPawnOwner(),
		WeaponItemInfo.Damage,
		WeaponItemInfo.WeaponItemType);

	// additional trigger damage
	TotalDamage += GetAdditionalDamage();
	CreateDamage(HitResult.GetActor(), TotalDamage, HitResult);
}


void ANakedWeapon::RemoveDelegate()
{
	for (const TPair<ENakedWeaponTriggerType, TArray<class ANakedWeaponTrigger*>>& Pair : NakedWeaponTriggerMap)
	{
		for (class ANakedWeaponTrigger* Actor : Pair.Value)
		{
			if (!Actor)
			{
				continue;
			}
			Actor->WeaponTriggerHitDelegate.RemoveDynamic(this, &ANakedWeapon::OnNakedTriggerHitDelegate);
		}
	}
}
#pragma endregion


void ANakedWeapon::DoDeployTemplate()
{
	if (Wevet::ArrayExtension::NullOrEmpty(TriggerTemplates))
	{
		UE_LOG(LogWevetClient, Error, TEXT("Empty Template => %s"), *FString(__FUNCTION__));
		return;
	}

	// @TODO
	// PawnClass needs to develop Interface.
	auto OwnerMeshComponent = (GetPawnOwner()->GetClass() == ACharacter::StaticClass()) ? 
		Cast<ACharacter>(GetPawnOwner())->GetMesh() : nullptr;

	if (!OwnerMeshComponent)
	{
		return;
	}

	// Deploy Template
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetPawnOwner();
	const FTransform Transform = GetPawnOwner()->GetActorTransform();
	for (auto Template : TriggerTemplates)
	{
		if (!Template)
		{
			continue;
		}

		ANakedWeaponTrigger* NakedWeaponTrigger = GetWorld()->SpawnActor<ANakedWeaponTrigger>(Template, Transform, SpawnParams);

		if (!NakedWeaponTrigger)
		{
			continue;
		}

		NakedWeaponTrigger->Initialize(IgnoreActors);
		const FName AttachName = NakedWeaponTrigger->GetAttachBoneName();
		const ENakedWeaponTriggerType TriggerType = NakedWeaponTrigger->GetNakedWeaponTriggerType();

		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		NakedWeaponTrigger->AttachToComponent(OwnerMeshComponent, Rules, AttachName);
		NakedWeaponTrigger->WeaponTriggerHitDelegate.AddDynamic(this, &ANakedWeapon::OnNakedTriggerHitDelegate);

		NakedWeaponTriggerMap.FindOrAdd(TriggerType).Add(NakedWeaponTrigger);

	}

	// output log
	{
		for (const TPair<ENakedWeaponTriggerType, TArray<class ANakedWeaponTrigger*>>& Pair : NakedWeaponTriggerMap)
		{
			UE_LOG(LogWevetClient, Log, TEXT("Type => %s"), *GETENUMSTRING("ENakedWeaponTriggerType", Pair.Key));

			for (class ANakedWeaponTrigger* Actor : Pair.Value)
			{
				if (Actor)
				{
					UE_LOG(LogWevetClient, Log, TEXT("Actor => %s"), *Actor->GetName());
				}
			}
		}
	}

}


/// <summary>
/// API From Combat Interface
/// </summary>
/// <param name="NakedWeaponTriggerType"></param>
/// <param name="Enable"></param>
/// <param name="FoundResult"></param>
void ANakedWeapon::NakedActionApply(const ENakedWeaponTriggerType NakedWeaponTriggerType, const bool Enable, bool& FoundResult)
{
	if (!Wevet::ArrayExtension::NullOrEmpty(NakedTriggerArray))
	{
		NakedTriggerArray.Reset(0);
	}

	NakedTriggerArray = *NakedWeaponTriggerMap.Find(NakedWeaponTriggerType);
	if (Wevet::ArrayExtension::NullOrEmpty(NakedTriggerArray))
	{
		return;
	}

	int32 Index = 0;
	const int32 MaxIndex = NakedTriggerArray.Num();
	for (class ANakedWeaponTrigger* WeaponTrigger : NakedTriggerArray)
	{
		if (WeaponTrigger)
		{
			WeaponTrigger->NakedActionApply(Enable);
			++Index;
		}
	}
	FoundResult = (Index == MaxIndex);
}


void ANakedWeapon::ClearNakedActionApply()
{
	for (const TPair<ENakedWeaponTriggerType, TArray<class ANakedWeaponTrigger*>>& Pair : NakedWeaponTriggerMap)
	{
		for (class ANakedWeaponTrigger* NakedWeaponTrigger : Pair.Value)
		{
			if (!NakedWeaponTrigger)
			{
				continue;
			}
			NakedWeaponTrigger->NakedActionApply(false);
		}
	}
}


void ANakedWeapon::OnNakedTriggerHitDelegate(AActor* OtherActor, const FHitResult SweepResult)
{
	if (!CanStrike_Implementation())
	{
		return;
	}

	const bool bCanDamageResult = UCombatBlueprintFunctionLibrary::CanDamagedActor(SweepResult.GetActor(), this, GetPawnOwner());
	if (!bCanDamageResult)
	{
		return;
	}

	GiveDamageType = EGiveDamageType::Melee;
	TakeDamageOuter(SweepResult);
	const FVector ImpactLocation = SweepResult.ImpactNormal;
	//ISoundInstigator::Execute_ReportNoiseOther(GetPawnOwner(), this, ImpactSound, DEFAULT_VOLUME, ImpactLocation);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), ImpactLocation, DEFAULT_VOLUME, this, IMPACT_RANGE);
}


float ANakedWeapon::GetAdditionalDamage() const
{
	if (Wevet::ArrayExtension::NullOrEmpty(NakedTriggerArray))
	{
		return DEFAULT_VALUE;
	}

	float Total = 0.0f;
	for (class ANakedWeaponTrigger* WeaponTrigger : NakedTriggerArray)
	{
		if (WeaponTrigger)
		{
			Total += WeaponTrigger->GetAddtionalDamage();
		}
	}
	return Total;
}

