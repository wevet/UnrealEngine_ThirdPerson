// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

// ctor
UWeapon::UWeapon(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

// ctor
FWeaponItemInfo::FWeaponItemInfo()
{
	this->EquipSocketName = FName(TEXT("Lancer_Root_Socket"));
}


