// Fill out your copyright notice in the Description page of Project Settings.

#include "WayPointBase.h"


AWayPointBase::AWayPointBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWayPointBase::BeginPlay()
{
	Super::BeginPlay();	
}

void AWayPointBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

