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

float UCharacterModel::GetHealthToWidget() const
{
	return (float)Health / (float)MaxHealth;
}

bool UCharacterModel::IsHealthHalf() const
{
	float Value = ((float)MaxHealth * HALF_WEIGHT);
	int32 HalfValue = (int32)Value;
	return Health < HalfValue;
}

bool UCharacterModel::IsHealthQuarter() const
{
	float Value = ((float)MaxHealth * QUART_WEIGHT);
	int32 QuartValue = (int32)Value;
	return Health < QuartValue;
}

void UCharacterModel::TakeDamage(const int32 InDamage)
{
	const int32 CurrentHealth = GetHealth();
	SetHealth(CurrentHealth - InDamage);
}

bool UCharacterModel::IsEmptyHealth() const
{
	return (Health <= INT_ZERO);
}

void UCharacterModel::Die()
{
	bDie = true;
}

bool UCharacterModel::IsDie() const
{
	return (Health <= INT_ZERO) && bDie;
}