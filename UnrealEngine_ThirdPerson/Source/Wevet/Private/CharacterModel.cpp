// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterModel.h"


UCharacterModel::UCharacterModel()
{

}

UCharacterModel::UCharacterModel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->CurrentHealth = 100;
	this->MaxHealth = 100;
	this->Attack = 100;
	this->Defence = 100;
	this->Wisdom = 100;
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
