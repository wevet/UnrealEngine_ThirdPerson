// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WayPointBase.generated.h"

UCLASS()
class WEVET_API AWayPointBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWayPointBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
};
