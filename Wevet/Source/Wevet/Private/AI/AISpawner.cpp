// Copyright 2018 wevet works All Rights Reserved.


#include "AI/AISpawner.h"
#include "Engine/World.h"
#include "Wevet.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AAISpawner::AAISpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnInterval = 1.0f;
	SpawnCount = 10;
	bSpawnFinished = false;
	SpawnRadius = 200.f;


	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetSoliderPath());
		CharacterTemplate = FindAsset.Object;
	}
}


void AAISpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SpawnPoints.Reset(0);
	Super::EndPlay(EndPlayReason);
}


void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
	StartSpawn();
}


void AAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpawnInterval >= SpawnTimer)
	{
		SpawnTimer += DeltaTime;
	}
	else
	{
		if (!bSpawnFinished)
		{
			SpawnTimer = ZERO_VALUE;
			DoSpawn();
		}
	}
}


void AAISpawner::StartSpawn()
{
	SpawnPoints.Reset(0);
	CurrentSpawnCount = 0;
	SpawnTimer = ZERO_VALUE;
	bSpawnFinished = false;
	UWevetBlueprintFunctionLibrary::CircleSpawnPoints(SpawnCount, SpawnRadius, GetActorLocation(), SpawnPoints);

	if (CharacterTemplate == nullptr)
	{
		UE_LOG(LogWevetClient, Error, TEXT("Not Setting BPTemplate : %s"), *FString(__FUNCTION__));
		Super::SetActorTickEnabled(false);
		return;
	}
	Super::SetActorTickEnabled(true);
}


void AAISpawner::DoSpawn()
{
	bSpawnFinished = (CurrentSpawnCount >= SpawnCount);
	if (bSpawnFinished)
	{
		Super::SetActorTickEnabled(false);
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform;
	Transform.SetLocation(SpawnPoints[CurrentSpawnCount]);
	AAICharacterBase* SpawningObject = GetWorld()->SpawnActor<AAICharacterBase>(CharacterTemplate, Transform, SpawnParams);

	if (SpawningObject)
	{
		++CurrentSpawnCount;

#if WITH_EDITOR
		SpawningObject->SetFolderPath("Enemy");
#endif

	}
}

