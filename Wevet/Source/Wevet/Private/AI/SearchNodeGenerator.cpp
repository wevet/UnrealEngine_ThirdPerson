// Copyright 2018 wevet works All Rights Reserved.


#include "AI/SearchNodeGenerator.h"

ASearchNodeGenerator::ASearchNodeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Tool/BP_SearchNode.BP_SearchNode_C"));
		SearchNodeTemplate = FindAsset.Object;
	}

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
