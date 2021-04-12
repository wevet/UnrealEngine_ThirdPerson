// Copyright 2018 wevet works All Rights Reserved.

#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "AI/WayPointBase.h"
#include "Player/MockCharacter.h"

#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "WevetExtension.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Singleton/BattleGameManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SearchNodeHolderKeyName = (FName(TEXT("SearchNodeHolder")));
	PatrolPointsHolderKeyName = (FName(TEXT("PatrolPointsHolder")));
	PatrolLocationKeyName = (FName(TEXT("PatrolLocation")));
	DestinationKeyName = (FName(TEXT("Destination")));
	ActionStateKeyName = (FName(TEXT("ActionState")));
	TargetKeyName = (FName(TEXT("Target")));

	SightConfig = nullptr;
	HearConfig  = nullptr;
	PredictionConfig = nullptr;

	BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTreeComponent"));
	BlackboardComponent   = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));
	AIPerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("AIPerceptionComponent"));

	HearConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Hearing>(this, TEXT("HearConfig"));
	HearConfig->HearingRange = 800.f;
	HearConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComponent->ConfigureSense(*HearConfig);

	SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 3000.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->SetMaxAge(20.f);
	AIPerceptionComponent->ConfigureSense(*SightConfig);

	PredictionConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Prediction>(this, TEXT("PredictionConfig"));
	AIPerceptionComponent->ConfigureSense(*PredictionConfig);

	DamageConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Damage>(this, TEXT("DamageConfig"));
	AIPerceptionComponent->ConfigureSense(*DamageConfig);

	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	SetGenericTeamId(TeamId);
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	if (Wevet::ComponentExtension::HasValid(AIPerceptionComponent))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdatedRecieve);
	}
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Character = Cast<AAICharacterBase>(InPawn);

	if (Character == nullptr)
	{
		return;
	}

	BlackboardComponent->InitializeBlackboard(*Character->GetBehaviorTree()->BlackboardAsset);
	UWevetBlueprintFunctionLibrary::GetWorldWayPointsArray(InPawn, FLT_MAX, WayPointList);
	BehaviorTreeComponent->StartTree(*Character->GetBehaviorTree());

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(Character))
	{
		// AddDelegate Death
		{
			FCombatDelegate* Delegate = Interface->GetDeathDelegate();
			if (Delegate)
			{
				(*Delegate).AddDynamic(this, &AAIControllerBase::OnDeath);
			}
		}
		// AddDelegate Kill
		{
			FCombatOneDelegate* Delegate = Interface->GetKillDelegate();
			if (Delegate)
			{
				(*Delegate).AddDynamic(this, &AAIControllerBase::OnKill);
			}
		}
	}
}

void AAIControllerBase::OnUnPossess()
{
	if (Wevet::ComponentExtension::HasValid(AIPerceptionComponent))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdatedRecieve);
	}

	StopTree();

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(Character))
	{
		// RemoveDelegate Death
		{
			FCombatDelegate* Delegate = Interface->GetDeathDelegate();
			if (Delegate)
			{
				(*Delegate).RemoveDynamic(this, &AAIControllerBase::OnDeath);
			}
		}
		// RemoveDelegate Kill
		{
			FCombatOneDelegate* Delegate = Interface->GetKillDelegate();
			if (Delegate)
			{
				(*Delegate).RemoveDynamic(this, &AAIControllerBase::OnKill);
			}
		}
	}

	Character = nullptr;
	Super::OnUnPossess();
}

void AAIControllerBase::StopTree()
{
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}
}

void AAIControllerBase::ResumeTree()
{
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->RestartTree();
	}
}

ETeamAttitude::Type AAIControllerBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);

	if (OtherTeamAgent)
	{
		// UE_LOG(LogTemp, Warning, TEXT("OtherName : %s, TeamID : %d"), *Other.GetFName().ToString(), OtherTeamAgent->GetGenericTeamId().GetId());
		// Neutral if team ID is 255 (NoTeam)
		if (OtherTeamAgent->GetGenericTeamId() == FGenericTeamId::NoTeam)
		{
			return ETeamAttitude::Neutral;
		}
		return FGenericTeamId::GetAttitude(GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId());
	}

	return ETeamAttitude::Neutral;
}

AWayPointBase* AAIControllerBase::GetWayPoint() const
{
	if (Wevet::ArrayExtension::NullOrEmpty(WayPointList))
	{
		return nullptr;
	}
	const int32 LastIndex = (WayPointList.Num() - 1);
	const int32 RandomIndex = FMath::RandRange(0, LastIndex);
	return WayPointList[RandomIndex];
}

#pragma region Blackboard
void AAIControllerBase::SetBlackboardTarget(AActor* const NewTarget)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(TargetKeyName, NewTarget);
	}
}

void AAIControllerBase::SetBlackboardSearchNodeHolder(AActor* const NewSearchNodeHolder)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(SearchNodeHolderKeyName, NewSearchNodeHolder);
	}
}

void AAIControllerBase::SetBlackboardPatrolPointsHolder(AActor* const NewPatrolPointsHolder)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(PatrolPointsHolderKeyName, NewPatrolPointsHolder);
	}
}

void AAIControllerBase::SetBlackboardPatrolLocation(const FVector NewLocation)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsVector(PatrolLocationKeyName, NewLocation);
	}
}

void AAIControllerBase::SetBlackboardDestinationLocation(const FVector NewDestination)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsVector(DestinationKeyName, NewDestination);
	}
}

void AAIControllerBase::SetBlackboardActionState(const EAIActionState NewAIActionState)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsEnum(ActionStateKeyName, (uint8)NewAIActionState);
	}
}
#pragma endregion

bool AAIControllerBase::WasBlackboardTargetDeath() const
{
	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(GetBlackboardTarget()))
	{
		return Interface->IsDeath_Implementation();
	}
	return false;
}

bool AAIControllerBase::WasKilledCrew(AActor* const InActor) const
{
	if (Character == nullptr || InActor == nullptr)
	{
		return false;
	}

	if ((InActor->GetClass() == Character->GetClass()))
	{
		ICombatInstigator* Interface = Cast<ICombatInstigator>(InActor);
		if (Interface && Interface->IsDeath_Implementation())
		{
			return true;
		}
	}
	return false;
}

void AAIControllerBase::OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus)
{
	if (Character == nullptr || ICombatInstigator::Execute_IsDeath(Character) || Actor == nullptr)
	{
		return;
	}

	// same perception Actor
	if (Character == Actor)
	{
		return;
	}

	// not safety data
	if (!Stimulus.IsValid())
	{
		return;
	}

	if (ILocomotionSystemPawn::Execute_GetALSMovementMode(Character) == ELSMovementMode::Ragdoll)
	{
		return;
	}

	CurrentStimulus = Stimulus;
	const FAISenseID CurrentSenseID = CurrentStimulus.Type;

	// Is the Actor dead with a companion?
	const bool bWasSameClass = (Character->GetClass() == Actor->GetClass());
	bool bWasKilledCrew = false;
	bool bWasTargetLive = false;
	if (bWasSameClass)
	{
		bWasKilledCrew = WasKilledCrew(Actor);
		if (bWasKilledCrew)
		{
			UE_LOG(LogWevetClient, Warning, TEXT("Found Same Class AI"));
		}
		else
		{
			// Comminucate AI
		}
	}
	else
	{
		if (WasBlackboardTargetDeath())
		{
			if (GetBlackboardTarget())
			{
				SetBlackboardTarget(nullptr);
				UE_LOG(LogWevetClient, Log, TEXT("Target Death"));
				return;
			}
		}
		else
		{
			if (!GetBlackboardTarget())
			{
				bWasTargetLive = true;
			}
		}
	}

	if (CurrentSenseID == UAISense::GetSenseID(SightConfig->GetSenseImplementation()))
	{
		if (bWasTargetLive)
		{
			ICombatInstigator::Execute_DoSightReceive(Character, Actor, CurrentStimulus, bWasKilledCrew);
		}
	}
	else if (CurrentSenseID == UAISense::GetSenseID(HearConfig->GetSenseImplementation()))
	{
		// I haven't found a Player, but I've detected a sound, so I'm on alert.
		if (!bWasSameClass)
		{
			ICombatInstigator::Execute_DoHearReceive(Character, Actor, CurrentStimulus, bWasKilledCrew);
		}
	}
	else if (CurrentSenseID == UAISense::GetSenseID(PredictionConfig->GetSenseImplementation()))
	{
		// @TODO
		// Predict the position of the other party.
		ICombatInstigator::Execute_DoPredictionReceive(Character, Actor, CurrentStimulus);
	}
	else if (CurrentSenseID == UAISense::GetSenseID(DamageConfig->GetSenseImplementation()))
	{
		// SendDamage
		ICombatInstigator::Execute_DoDamageReceive(Character, Actor, CurrentStimulus);
	}

}

const TArray<FVector>& AAIControllerBase::GetPathPointArray()
{
	PointsArray.Reset(0);
	if (BlackboardComponent == nullptr || Character == nullptr)
	{
		return PointsArray;
	}

	const FVector ActorLocation = Character->GetActorLocation();
	UNavigationPath* NavPath = nullptr;

	AActor* TargetActor = GetBlackboardTarget();
	if (TargetActor)
	{
		NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), ActorLocation, TargetActor);
	}
	else
	{
		const FVector TargetLocation = BlackboardComponent->GetValueAsVector(PatrolLocationKeyName);
		NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), ActorLocation, TargetLocation);
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

void AAIControllerBase::RemoveSearchNodeGenerator()
{
	if (Character)
	{
		Character->RemoveSearchNodeGenerator();
	}
}

void AAIControllerBase::CheckTargetStatus(bool& OutResult)
{
	OutResult = (GetBlackboardTarget() != nullptr);
	if (OutResult)
	{
		if (WasBlackboardTargetDeath())
		{
			SetBlackboardTarget(nullptr);
			UE_LOG(LogWevetClient, Warning, TEXT("Did you die during Combat? => Owner : %s"), *Character->GetName());
		}
	}
}

void AAIControllerBase::OnDeath()
{
	UE_LOG(LogWevetClient, Log, TEXT("Death : %s"), *FString(__FUNCTION__));
	SetBlackboardTarget(nullptr);
}

void AAIControllerBase::OnKill(AActor* InActor)
{
	UE_LOG(LogWevetClient, Log, TEXT("Kill : %s"), *FString(__FUNCTION__));
	SetBlackboardTarget(nullptr);
}

void AAIControllerBase::BattlePhaseUpdate()
{
	const bool bWasHeard = true;
	const bool bWasSee = true;
	Wevet::BattlePhase BP = Wevet::BattlePhase::Normal;
	if (bWasSee)
	{
		BP = Wevet::BattlePhase::Alert;
	}
	else if (!bWasSee && bWasHeard)
	{
		BP = Wevet::BattlePhase::Warning;
	}
	ABattleGameManager::GetInstance()->SetBattlePhase(BP);
}

