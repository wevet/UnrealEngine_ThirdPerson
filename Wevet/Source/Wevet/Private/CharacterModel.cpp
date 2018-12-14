// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterModel.h"


UCharacterModel::UCharacterModel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CurrentHealth(100), 
	MaxHealth(100), 
	Attack(100), 
	Defence(100), 
	Wisdom(100)
{
	//
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
