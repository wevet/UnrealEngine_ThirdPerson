// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MockPlayerController.h"
#include "WevetGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API AWevetGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AWevetGameModeBase();
	AWevetGameModeBase(const FObjectInitializer& ObjectInitializer);

	virtual void InitGameState() override;
	virtual void StartPlay() override;
	

};
