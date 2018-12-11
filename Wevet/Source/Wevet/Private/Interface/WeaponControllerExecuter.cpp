// Copyright 2018 wevet works All Rights Reserved.

#include "WeaponControllerExecuter.h"

UWeaponControllerExecuter::UWeaponControllerExecuter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FWeaponItemInfo::FWeaponItemInfo()
{
	this->EquipSocketName = FName(TEXT("Lancer_Root_Socket"));
	this->Damage = 20.f;
}


