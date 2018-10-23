// Copyright 2018 wevet works All Rights Reserved.

#include "BulletBase.h"


// Sets default values
ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

