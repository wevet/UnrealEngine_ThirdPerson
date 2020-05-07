// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterModel.h"
#include "Wevet.h"

UCharacterModel::UCharacterModel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Health(100), 
	MaxHealth(100), 
	Attack(100), 
	Defence(100), 
	Wisdom(100), 
	bDie(false)
{
}

void UCharacterModel::SetHealth(const int32 NewHealth)
{
	Health = FMath::Clamp<int32>(NewHealth, INT_ZERO, MaxHealth);
}

void UCharacterModel::SetMaxHealth(const int32 NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

void UCharacterModel::SetAttack(const int32 NewAttack)
{
	Attack = NewAttack;
}

void UCharacterModel::SetDefence(const int32 NewDefence)
{
	Defence = NewDefence;
}

void UCharacterModel::SetWisdom(const int32 NewWisdom)
{
	Wisdom = NewWisdom;
}

void UCharacterModel::TakeDamage(const int32 InDamage)
{
	const int32 CurrentHealth = GetHealth();
	SetHealth(CurrentHealth - InDamage);
}

void UCharacterModel::Recover(const int32 AddHealth)
{
	Health += AddHealth;
	Health = FMath::Clamp<int32>(Health, INT_ZERO, MaxHealth);
}
