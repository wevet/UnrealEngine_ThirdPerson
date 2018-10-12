// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponControllerExecuter.h"

UWeaponControllerExecuter::UWeaponControllerExecuter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FWeaponItemInfo::FWeaponItemInfo()
{
	this->EquipSocketName = FName(TEXT("Lancer_Root_Socket"));
}


