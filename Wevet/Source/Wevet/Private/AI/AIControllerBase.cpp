// Copyright 2018 wevet works All Rights Reserved.
#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "AI/WayPointBase.h"
#include "Player/MockCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "WevetExtension.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

using namespace Wevet;

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BotTypeKeyName = (FName(TEXT("BotType")));
	TargetKeyName = (FName(TEXT("Target")));
	CanSeePlayerKeyName   = (FName(TEXT("CanSeePlayer")));
	CanHearPlayerKeyName  = (FName(TEXT("CanHearPlayer")));
	PatrolLocationKeyName = (FName(TEXT("PatrolLocation")));
	ActionStateKeyName    = (FName(TEXT("ActionState")));
	SightConfig = nullptr;
	HearConfig  = nullptr;
	PredictionConfig = nullptr;

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

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AICharacterOwner = Cast<AAICharacterBase>(InPawn);

	if (AICharacterOwner)
	{
		AICharacterOwner->InitializePosses();
		BlackboardComponent->InitializeBlackboard(*AICharacterOwner->BehaviorTree->BlackboardAsset);
		UWevetBlueprintFunctionLibrary::GetWorldWayPointsArray(InPawn, FLT_MAX, WayPointList);
		BehaviorTreeComponent->StartTree(*AICharacterOwner->BehaviorTree);
	}
}

void AAIControllerBase::OnUnPossess()
{
	if (ComponentExtension::HasValid(AIPerceptionComponent))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdatedRecieve);
	}
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}
	Super::OnUnPossess();
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
	const int32 LastIndex = (WayPointList.Num() - 1);
	const int32 RandomIndex = FMath::RandRange(0, LastIndex);
	return WayPointList[RandomIndex];
}

void AAIControllerBase::SetBlackboardTarget(APawn* NewTarget)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(TargetKeyName, NewTarget);
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

void AAIControllerBase::SetBlackboardActionState(const EAIActionState NewAIActionState)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsEnum(ActionStateKeyName, (uint8)NewAIActionState);
	}
}

void AAIControllerBase::OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus)
{
	if (AICharacterOwner == nullptr || IDamageInstigator::Execute_IsDeath(AICharacterOwner))
	{
		return;
	}

	//bool bSuccess = (!IDamageInstigator::Execute_IsDeath(AICharacterOwner)) && Stimulus.WasSuccessfullySensed() ? true : false;
	//UE_LOG(LogWevetClient, Log, TEXT("WasSuccessfullySensed : %s"), bSuccess ? TEXT("True") : TEXT("false"));
}

const TArray<FVector>& AAIControllerBase::GetPathPointArray()
{
	PointsArray.Reset(0);
	UWorld* World = GetWorld();
	check(World);

	if (BlackboardComponent == nullptr || GetPawn() == nullptr)
	{
		UE_LOG(LogWevetClient, Error, TEXT("nullptr Pawn or BB : %s"), *FString(__FUNCTION__));
		return PointsArray;
	}

	APawn* ControllPawn = GetPawn();
	FVector TargetLocation = FVector::ZeroVector;
	UNavigationPath* NavPath = nullptr;

	if (IAIPawnOwner::Execute_IsSeeTarget(ControllPawn))
	{
		AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKeyName));
		if (Target)
		{

			NavPath = UNavigationSystemV1::FindPathToActorSynchronously(
				World,
				ControllPawn->GetActorLocation(),
				Target);
		}
		else
		{
			UE_LOG(LogWevetClient, Error, TEXT("nullptr TargetActor : %s"), *FString(__FUNCTION__));
		}
	}
	else
	{
		TargetLocation = BlackboardComponent->GetValueAsVector(PatrolLocationKeyName);
		NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			World,
			ControllPawn->GetActorLocation(),
			TargetLocation);
	}

	if (NavPath)
	{
		for (FVector P : NavPath->PathPoints)
		{
			PointsArray.Add(P);
		}
	}
	return PointsArray;
}

