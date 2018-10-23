// Copyright 2018 wevet works All Rights Reserved.

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
