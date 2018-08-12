// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacterActorBase.h"
#include "AICharacterBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class WEVET_API AAICharacterBase : public AAICharacterActorBase
{
	GENERATED_BODY()

public:
	AAICharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;
	virtual void Die_Implementation() override;
	virtual void NotifyEquip_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|ReadOnlyValiable")
	bool GetActivate() const { return this->Activate; }

	float GetAcceptanceRadius() const
	{
		return this->AcceptanceRadius;
	}

protected:
	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnSeePawnRecieve(APawn* OtherPawn) override;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume) override;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual void Scanning() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Valiable")
	bool Activate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Valiable")
	float AcceptanceRadius;

private:
	float TickWaitInterval = 0.5f;
	float TickInterval;
};

