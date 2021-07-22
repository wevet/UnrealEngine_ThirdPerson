// Copyright 2018 wevet works All Rights Reserved.


#include "Lib/CombatBlueprintFunctionLibrary.h"
#include "Character/CharacterBase.h"


#define WEAPON_DROP 20.f
#define ITEM_DROP 15.f


bool UCombatBlueprintFunctionLibrary::WeaponDropProbability()
{
	return Probability(WEAPON_DROP);
}


bool UCombatBlueprintFunctionLibrary::ItemDropProbability()
{
	return Probability(ITEM_DROP);
}


bool UCombatBlueprintFunctionLibrary::Probability(const float InPercent)
{
	const float Max = 100.f;
	const float ProbabilityRate = FMath::RandRange(0.0f, Max);
	if (InPercent == Max && ProbabilityRate == InPercent)
	{
		return true;
	}
	else if (ProbabilityRate < InPercent)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool UCombatBlueprintFunctionLibrary::CanDamagedActor(AActor* OtherActor, AActor* InSelf, APawn* InOwner)
{
	if (OtherActor == nullptr || InOwner == nullptr)
	{
		return false;
	}

	// Same Actor or Hit Actor Owner or PendingKilling
	if ((OtherActor == InSelf) || (OtherActor == InOwner) || OtherActor->IsPendingKill())
	{
		return false;
	}

	// if SameClassess
	if (OtherActor->IsA(InOwner->GetClass()))
	{
		return false;
	}

	// Attacks are possible if the actor's Damage flag is enabled
	const bool bCanBeDamagedActor = OtherActor->CanBeDamaged();
	const bool bHasDamagedTagActor = OtherActor->ActorHasTag(DAMAGE_TAG);
	if (OtherActor->GetClass()->ImplementsInterface(UCombatInstigator::StaticClass()))
	{
		return !ICombatInstigator::Execute_IsDeath(OtherActor);
	}
	else if (bCanBeDamagedActor || bHasDamagedTagActor)
	{
		return true;
	}

	return false;
}


/// <summary>
/// Calcurate BaseDamage
/// </summary>
/// <param name="HitResult"></param>
/// <param name="InSelf"></param>
/// <param name="InOwner"></param>
/// <param name="InBaseDamage"></param>
/// <returns></returns>
float UCombatBlueprintFunctionLibrary::CalcurateBaseDamage(const FHitResult& HitResult, AActor* InSelf, APawn* InOwner, const float InBaseDamage)
{
	float TotalDamage = ZERO_VALUE;
	AActor* OtherActor = HitResult.GetActor();
	if (OtherActor == nullptr || InOwner == nullptr)
	{
		return TotalDamage;
	}

	// Same Actor or Hit Actor Owner or PendingKilling
	if ((OtherActor == InSelf) || (OtherActor == InOwner) || OtherActor->IsPendingKill())
	{
		return TotalDamage;
	}

	ICombatInstigator* CombatInstigator = Cast<ICombatInstigator>(OtherActor);
	if (CombatInstigator)
	{
		// Calculate the basic damage value of Pawn.
		UCharacterModel* const DamageModel = ICombatInstigator::Execute_GetPropertyModel(CombatInstigator->_getUObject());
		const bool bCanKillDamage = ICombatInstigator::Execute_CanKillDealDamage(CombatInstigator->_getUObject(), HitResult.BoneName);
		TotalDamage = ICombatInstigator::Execute_MakeDamage(InOwner, DamageModel, InBaseDamage);

		if (bCanKillDamage)
		{
			TotalDamage += (float)DamageModel->GetKillDamage();
		}
	}
	else
	{
		TotalDamage = InBaseDamage;
	}


	IDamageTypeInstigator* DamageTypeInstigator = Cast<IDamageTypeInstigator>(InSelf);
	const EGiveDamageType GiveDamageType = DamageTypeInstigator ? 
		IDamageTypeInstigator::Execute_GetGiveDamageType(DamageTypeInstigator->_getUObject()) : EGiveDamageType::None;

	switch (GiveDamageType)
	{
		case EGiveDamageType::Shoot:
		{
			//
		}
		break;
		case EGiveDamageType::Melee:
		{
			//
		}
		break;
	}


	if (CombatInstigator)
	{
		ICombatInstigator::Execute_HitEffectReceive(OtherActor, HitResult, GiveDamageType);
	}

	return TotalDamage;
}
