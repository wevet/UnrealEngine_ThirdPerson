// Copyright 2018 wevet works All Rights Reserved.


#include "AI/SearchNodeGenerator.h"
#include "WevetExtension.h"

ASearchNodeGenerator::ASearchNodeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetSearchNodePath());
	SearchNodeTemplate = FindAsset.Object;

}

void ASearchNodeGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void ASearchNodeGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GridNodes.Reset(0);
	PrepareDestroy();
	Super::EndPlay(EndPlayReason);
}

void ASearchNodeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASearchNodeGenerator::BeginDestroy()
{
	PrepareDestroy();
	Super::BeginDestroy();
}

void ASearchNodeGenerator::PrepareDestroy()
{
	for (AActor* Actor : GridNodes)
	{
		if (Actor == nullptr)
		{
			continue;
		}
		Actor->Destroy();
	}
}
