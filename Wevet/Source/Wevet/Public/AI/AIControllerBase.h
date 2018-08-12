// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerActorBase.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "CharacterBase.h"
#include "AICharacterBase.h"
#include "WayPointBase.h"
#include "BulletBase.h"
#include "AIControllerBase.generated.h"

/**
*
*/
UCLASS()
class WEVET_API AAIControllerBase : public AAIControllerActorBase
{
	GENERATED_BODY()


public:

	AAIControllerBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Action")
	virtual void CreateTimerFunc();

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Valiable")
	ACharacterBase* GetTargetCharacter() const;

	virtual void Patrolling_Implementation() override;
	virtual void CheckEnemySighting_Implementation() override;
	virtual void Hunting_Implementation() override;


	bool HasCheckEnemyResult() const 
	{
		return this->CheckEnemyResult; 
	}
	
protected:

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Valiable")
	virtual void SetupAI();

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Valiable")
	virtual void OnFirePress();

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Valiable")
	virtual void OnFireRelease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	AAICharacterBase * CharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	TArray<AWayPointBase*> WayPointArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	int32 WayPointIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	FTimerHandle TimerFunc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	FString FunctionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	TSubclassOf<class ABulletBase> BulletsBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Valiable")
	bool IsWalkBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Valiable")
	float AcceptanceRadius;

	FTimerHandle AlternateFunc;
	bool CheckEnemyResult;

};

