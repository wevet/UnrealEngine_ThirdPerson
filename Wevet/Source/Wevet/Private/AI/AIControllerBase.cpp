// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerBase.h"
#include "Engine.h"


AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	WayPointIndex(0),
	IsWalkBack(false)
{
	this->FunctionName = FString(TEXT("CheckEnemySighting"));
}

void AAIControllerBase::CreateTimerFunc()
{
	//this->TimerFunc = SetTimerByFunctionName();
	//GetWorld()->GetTimerManager().SetTimer();
}

ACharacterBase* AAIControllerBase::GetTargetCharacter() const
{
	return this->CharacterRef->GetPlayerCharacter();
}

void AAIControllerBase::Patrolling_Implementation()
{
	if (this->CharacterRef->IsDeath_Implementation()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAlternateFunc"));
		GetWorld()->GetTimerManager().ClearTimer(this->AlternateFunc);
	}
	else 
	{
		// @TODO
		//if (this->CharacterRef->IsEnemyFound) 
		//{
		//	return;
		//}

		//AWayPointBase* Current = NULL;
		//
		//if (this->IsWalkBack) 
		//{
		//	--this->WayPointIndex;
		//}
		//else 
		//{
		//	++this->WayPointIndex;
		//}
		//Current = this->WayPointArray[this->WayPointIndex];

		//EPathFollowingRequestResult::Type RequsetType = MoveToActor(Current, -1.f, true, true, true, DefaultNavigationFilterClass, true);

		//if (RequsetType == EPathFollowingRequestResult::Type::RequestSuccessful)
		//{
		//	if (this->WayPointIndex <= 0) 
		//	{
		//		this->IsWalkBack = false;
		//	}
		//	if (this->WayPointIndex >= this->WayPointArray.Num() - 1)
		//	{
		//		this->IsWalkBack = true;
		//	}
		//}
	}
}

void AAIControllerBase::CheckEnemySighting_Implementation()
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (this->CharacterRef->IsDeath_Implementation())
	{
		World->GetTimerManager().ClearTimer(this->TimerFunc);
		Super::StopMovement();
		return;
	}

	if (this->GetTargetCharacter()->IsDeath_Implementation()) 
	{
		return;
	}

	if (this->CharacterRef->GetEnemyFound())
	{
		Hunting_Implementation();
	}
	else 
	{

	}
}

void AAIControllerBase::Hunting_Implementation()
{
	Super::Hunting_Implementation();
}

void AAIControllerBase::SetupAI()
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (Super::AICharacterOwner)
	{
		this->CharacterRef  = Cast<AAICharacterBase>(Super::AICharacterOwner);
		this->AcceptanceRadius = this->CharacterRef->GetAcceptanceRadius();
	}
	World->GetTimerManager().SetTimer(this->AlternateFunc, this, &AAIControllerBase::Patrolling_Implementation, 0.5f, true);
}

void AAIControllerBase::OnFirePress()
{
	if (Super::AICharacterOwner->GetSelectedWeapon() == nullptr)
	{
		return;
	}

	Super::AICharacterOwner->GetSelectedWeapon()->OnFirePressedInternal();
}

void AAIControllerBase::OnFireRelease()
{
	if (Super::AICharacterOwner->GetSelectedWeapon() == nullptr)
	{
		return;
	}

	Super::AICharacterOwner->GetSelectedWeapon()->OnFireReleaseInternal();
}