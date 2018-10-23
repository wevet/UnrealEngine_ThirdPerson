// Copyright 2018 wevet works All Rights Reserved.

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

