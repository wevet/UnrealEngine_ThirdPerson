// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WevetTypes.h"
#include "Components/PrimitiveComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NakedWeaponTrigger.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNakedHitDelegate, AActor* const, OtherActor, FHitResult const, SweepResult);


UCLASS()
class WEVET_API ANakedWeaponTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ANakedWeaponTrigger(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	ENakedWeaponTriggerType NakedWeaponTriggerType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	FName AttachBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	float AddtionalDamage;

	class UPrimitiveComponent* PrimitiveComponent;

	TArray<class AActor*> IgnoreActors;
	bool bWasHitResult;
	bool bWasOverlapResult;


protected:
	UFUNCTION()
	void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:
	UPROPERTY(BlueprintAssignable)
	FNakedHitDelegate NakedHitDelegate;

	ENakedWeaponTriggerType GetNakedWeaponTriggerType() const
	{
		return NakedWeaponTriggerType;
	}

	FORCEINLINE float GetAddtionalDamage() const 
	{
		return AddtionalDamage;
	}

	void SetOwners(const TArray<class AActor*>& InOwners);
};
