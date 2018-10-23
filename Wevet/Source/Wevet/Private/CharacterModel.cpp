// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterModel.h"


UCharacterModel::UCharacterModel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CurrentHealth(100), MaxHealth(100), Attack(100), Defence(100), Wisdom(100)
{
	//
}

void UCharacterModel::SetCurrentHealthValue(int32 Health)
{
	this->CurrentHealth = Health;
}

void UCharacterModel::SetMaxHealthValue(int32 Health)
{
	this->MaxHealth = Health;
}

void UCharacterModel::SetAttackValue(int32 Attack)
{
	this->Attack = Attack;
}

void UCharacterModel::SetDefenceValue(int32 Defence)
{
	this->Defence = Defence;
}

void UCharacterModel::SetWisdomValue(int32 Wisdom)
{
	this->Wisdom = Wisdom;
}
