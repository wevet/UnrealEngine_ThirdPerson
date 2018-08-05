// Fill out your copyright notice in the Description page of Project Settings.

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

