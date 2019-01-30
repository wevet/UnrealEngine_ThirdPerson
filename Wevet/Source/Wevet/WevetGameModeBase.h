// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WevetGameModeBase.generated.h"


class ACharacterBase;
class AMockCharacter;

UCLASS()
class WEVET_API AWevetGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AWevetGameModeBase(const FObjectInitializer& ObjectInitializer);

	virtual void InitGameState() override;
	virtual void StartPlay() override;
	

private:
	TArray<ACharacterBase*> CharacterArray;
	AMockCharacter* Player;
};
