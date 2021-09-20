// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "WeaponTriggerBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponTriggerHitDelegate, AActor* const, OtherActor, FHitResult const, SweepResult);


UCLASS()
class WEVET_API AWeaponTriggerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponTriggerBase(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;


public:
	UPROPERTY(BlueprintAssignable)
	FWeaponTriggerHitDelegate WeaponTriggerHitDelegate;

	virtual void Initialize(const TArray<class AActor*>& InOwners);


protected:
	UFUNCTION()
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


protected:
	class UPrimitiveComponent* PrimitiveComponent;
	TArray<class AActor*> IgnoreActors;
	bool bWasHitResult;
	bool bWasOverlapResult;

};

