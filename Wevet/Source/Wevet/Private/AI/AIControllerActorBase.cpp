// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerActorBase.h"
#include "AICharacterActorBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"

AAIControllerActorBase::AAIControllerActorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
	//AIPerceptionComponent->SetDominantSense(HearConfig->GetSenseImplementation());
	this->CanSeePlayerKey = FName(TEXT("CanSeePlayer"));
}

void AAIControllerActorBase::Possess(APawn * Pawn)
{
	Super::Possess(Pawn);

	this->AICharacterOwner = Cast<AAICharacterActorBase>(Pawn);
	if (this->AICharacterOwner)
	{
		UBlackboardData* BlackBoard = this->AICharacterOwner->BehaviorTree->BlackboardAsset;
		if (BlackBoard)
		{
			//
		}

		this->AICharacterOwner->InitializePosses();
		this->WayPointArray = this->AICharacterOwner->GetWayPointList();
		Super::RunBehaviorTree(this->AICharacterOwner->BehaviorTree);
	}
}

FGenericTeamId AAIControllerActorBase::GetGenericTeamId() const
{
	return PTG_TEAM_ID_ENEMY;
}

void AAIControllerActorBase::Patrolling_Implementation()
{

}

void AAIControllerActorBase::CheckEnemySighting_Implementation()
{

}

void AAIControllerActorBase::Hunting_Implementation()
{
	//auto BB = UAIBlueprintHelperLibrary::GetBlackboard();
}

AWayPointBase* AAIControllerActorBase::GetRandomAtWayPoint()
{
	if (this->WayPointArray.Num() <= 0)
	{
		return nullptr;
	}
	int32 RandomIndex = FMath::RandRange(0, this->WayPointArray.Num() - 1);
	return this->WayPointArray[RandomIndex];
}

void AAIControllerActorBase::BeginPlay()
{
	Super::BeginPlay();
	if (!ensure(AIPerceptionComponent)) 
	{
		return;
	}
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerActorBase::OnTargetPerceptionUpdatedRecieve);
}

void AAIControllerActorBase::OnTargetPerceptionUpdatedRecieve(AActor * Actor, FAIStimulus Stimulus)
{
	AMockCharacter* MockCharacter = Cast<AMockCharacter>(Actor);

	if (MockCharacter)
	{
		if (MockCharacter->IsDeath_Implementation())
		{
			return;
		}
		if (GetBlackboardComponent()) 
		{
			auto BComp = GetBlackboardComponent();

			bool Success = Stimulus.WasSuccessfullySensed() ? true : false;
			BComp->SetValueAsBool(this->CanSeePlayerKey, Success);

			//UE_LOG(LogTemp, Warning, TEXT("PerceptionUpdated : %s"), Success ? TEXT("True") : TEXT("False"));

			if (this->AICharacterOwner)
			{
				this->AICharacterOwner->SetTargetActor(MockCharacter);
				this->AICharacterOwner->SetEnemyFound(Success);
			}
		}
	}
}


