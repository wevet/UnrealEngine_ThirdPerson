// Copyright 2018 wevet works All Rights Reserved.
#include "AIControllerBase.h"
#include "MockCharacter.h"
#include "AICharacterBase.h"
#include "WayPointBase.h"

#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	BotTypeKeyName(FName(TEXT("BotType"))),
	CanSeePlayerKeyName(FName(TEXT("CanSeePlayer"))),
	CanHearPlayerKeyName(FName(TEXT("CanHearPlayer"))),
	TargetEnemyKeyName(FName(TEXT("TargetEnemy"))),
	PatrolLocationKeyName(FName(TEXT("PatrolLocation"))),
	SightConfig(nullptr),
	HearConfig(nullptr),
	PredictionConfig(nullptr)
{

	BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTreeComponent"));
	BlackboardComponent   = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));
	AIPerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("AIPerceptionComponent"));

	SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
	SightConfig->SightRadius = 3000.f;
	SightConfig->LoseSightRadius = 3500.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals   = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComponent->ConfigureSense(*SightConfig);

	HearConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Hearing>(this, TEXT("HearConfig"));
	HearConfig->HearingRange = 1000.f;
	HearConfig->DetectionByAffiliation.bDetectEnemies    = true;
	HearConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearConfig->DetectionByAffiliation.bDetectNeutrals   = true;
	AIPerceptionComponent->ConfigureSense(*HearConfig);

	PredictionConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Prediction>(this, TEXT("PredictionConfig"));
	AIPerceptionComponent->ConfigureSense(*PredictionConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	if (ComponentExtension::HasValid(AIPerceptionComponent))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdatedRecieve);
	}
}

void AAIControllerBase::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);
	AICharacterOwner = Cast<AAICharacterBase>(Pawn);
	bool bInitialized = false;
	if (AICharacterOwner)
	{
		bInitialized = BlackboardComponent->InitializeBlackboard(*AICharacterOwner->BehaviorTree->BlackboardAsset);
	}
	if (bInitialized)
	{
		AICharacterOwner->InitializePosses();
		AICharacterOwner->CreateWayPointList(WayPointList);
		BehaviorTreeComponent->StartTree(*AICharacterOwner->BehaviorTree);
	}
}

void AAIControllerBase::UnPossess()
{
	if (ComponentExtension::HasValid(AIPerceptionComponent))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdatedRecieve);
	}
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}
	Super::UnPossess();
}

FGenericTeamId AAIControllerBase::GetGenericTeamId() const
{
	return PTG_TEAM_ID_ENEMY;
}

AWayPointBase* AAIControllerBase::GetRandomAtWayPoint()
{
	if (ArrayExtension::NullOrEmpty(WayPointList))
	{
		return nullptr;
	}
	const int32 RandomIndex = FMath::RandRange(0, WayPointList.Num() - 1);
	return WayPointList[RandomIndex];
}

void AAIControllerBase::SetBlackboardTarget(APawn* NewTarget)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(TargetEnemyKeyName, NewTarget);
	}
}

void AAIControllerBase::SetBlackboardWayPoint(AWayPointBase* NewWayPoint)
{
	if (BlackboardComponent)
	{
		//@TODO
		//BlackboardComponent->SetValueAsObject(TargetEnemyKeyName, NewWayPoint);
	}
}

void AAIControllerBase::SetBlackboardBotType(EBotBehaviorType NewType)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsEnum(BotTypeKeyName, (uint8)NewType);
	}
}

void AAIControllerBase::SetBlackboardSeeActor(const bool NewCanSeeActor)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsBool(CanSeePlayerKeyName, NewCanSeeActor);
	}
}

void AAIControllerBase::SetBlackboardHearActor(const bool NewCanHearActor)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsBool(CanHearPlayerKeyName, NewCanHearActor);
	}
}

void AAIControllerBase::SetBlackboardPatrolLocation(const FVector NewLocation)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsVector(PatrolLocationKeyName, NewLocation);
	}
}

ACharacterBase* AAIControllerBase::GetTargetCharacter() const
{
	if (GetAICharacter())
	{
		return GetAICharacter()->GetTargetCharacter();
	}
	return nullptr;
}

void AAIControllerBase::OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus)
{
	if (AICharacterOwner == nullptr || ICombatExecuter::Execute_IsDeath(AICharacterOwner))
	{
		return;
	}
	if (!GetTargetCharacter())
	{
		return;
	}

	bool bSuccess = (!ICombatExecuter::Execute_IsDeath(GetTargetCharacter())) && Stimulus.WasSuccessfullySensed() ? true : false;
	//UE_LOG(LogWevetClient, Log, TEXT("WasSuccessfullySensed : %s"), bSuccess ? TEXT("True") : TEXT("false"));
}


