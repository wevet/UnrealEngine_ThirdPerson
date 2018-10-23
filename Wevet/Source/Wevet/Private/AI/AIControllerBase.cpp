// Copyright 2018 wevet works All Rights Reserved.

#include "AIControllerBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	CanSeePlayerKey(FName(TEXT("CanSeePlayer")))
{

	AIPerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("AIPerceptionComponent"));

	// sight create
	SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
	SightConfig->SightRadius = 3000.f;
	SightConfig->LoseSightRadius = 3500.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals   = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// hear create
	HearConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Hearing>(this, TEXT("HearConfig"));
	HearConfig->HearingRange = 1000.f;
	HearConfig->DetectionByAffiliation.bDetectEnemies    = true;
	HearConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearConfig->DetectionByAffiliation.bDetectNeutrals   = true;
	AIPerceptionComponent->ConfigureSense(*HearConfig);
	AIPerceptionComponent->SetDominantSense(HearConfig->GetSenseImplementation());
}

void AAIControllerBase::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	this->AICharacterOwner = Cast<AAICharacterBase>(Pawn);
	if (this->AICharacterOwner)
	{
		UBlackboardData* BlackBoard = this->AICharacterOwner->BehaviorTree->BlackboardAsset;
		if (BlackBoard)
		{
			//
		}

		this->AICharacterOwner->InitializePosses();
		this->WayPointList = this->AICharacterOwner->GetWayPointList();
		Super::RunBehaviorTree(this->AICharacterOwner->BehaviorTree);
	}
}

FGenericTeamId AAIControllerBase::GetGenericTeamId() const
{
	return PTG_TEAM_ID_ENEMY;
}

void AAIControllerBase::Patrolling_Implementation() {}

void AAIControllerBase::CheckEnemySighting_Implementation() {}

void AAIControllerBase::Hunting_Implementation() {}

AWayPointBase* AAIControllerBase::GetRandomAtWayPoint()
{
	if (this->WayPointList.Num() <= 0)
	{
		return nullptr;
	}
	int32 RandomIndex = FMath::RandRange(0, this->WayPointList.Num() - 1);
	return this->WayPointList[RandomIndex];
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	if (!ensure(AIPerceptionComponent))
	{
		return;
	}
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdatedRecieve);
}

void AAIControllerBase::OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus)
{
	if (this->AICharacterOwner == nullptr 
		|| (this->AICharacterOwner && this->AICharacterOwner->IsDeath_Implementation()))
	{
		return;
	}

	UBlackboardComponent* BComp = GetBlackboardComponent();
	AMockCharacter* MockCharacter = Cast<AMockCharacter>(Actor);

	if (MockCharacter == nullptr 
		|| (MockCharacter && MockCharacter->IsDeath_Implementation()))
	{
		return;
	}

	if (BComp)
	{
		if (this->AICharacterOwner->HasEnemyFound())
		{
			return;
		}

		bool Success = Stimulus.WasSuccessfullySensed() ? true : false;
		//UE_LOG(LogTemp, Warning, TEXT("PerceptionUpdated : %s"), Success ? TEXT("True") : TEXT("False"));
		this->AICharacterOwner->SetTargetActor(MockCharacter);
		this->AICharacterOwner->SetEnemyFound(Success);
		BComp->SetValueAsBool(this->CanSeePlayerKey, Success);
	}

}


