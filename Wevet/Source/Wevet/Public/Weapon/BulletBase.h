// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

UCLASS()
class WEVET_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	float LifeInterval;

public:	
	virtual void Tick(float DeltaTime) override;

	
	
};
