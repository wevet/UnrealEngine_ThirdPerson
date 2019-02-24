// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterModel.h"


UCharacterModel::UCharacterModel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CurrentHealth(100), 
	MaxHealth(100), 
	Attack(100), 
	Defence(100), 
	Wisdom(100), 
	bDie(false)
{
}

void UCharacterModel::SetCurrentHealthValue(int32 NewHealth)
{
	CurrentHealth = NewHealth;
}

void UCharacterModel::SetMaxHealthValue(int32 NewHealth)
{
	MaxHealth = NewHealth;
}

void UCharacterModel::SetAttackValue(int32 NewAttack)
{
	Attack = NewAttack;
}

void UCharacterModel::SetDefenceValue(int32 NewDefence)
{
	Defence = NewDefence;
}

void UCharacterModel::SetWisdomValue(int32 NewWisdom)
{
	Wisdom = NewWisdom;
}

float UCharacterModel::GetHealthToWidget() const
{
	return (float)CurrentHealth / (float)MaxHealth;
}

bool UCharacterModel::IsHealthHalf() const
{
	int32 HalfValue = (int32)(MaxHealth * 0.5f);
	return CurrentHealth > HalfValue;
}

bool UCharacterModel::IsHealthQuarter() const
{
	int32 HalfValue = (int32)(MaxHealth * 0.25f);
	return CurrentHealth > HalfValue;
}

void UCharacterModel::Die()
{
	bDie = true;
}
