// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "BulletBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHitTriggerDelegate, AActor* const, OtherActor, FHitResult const, SweepResult);

UCLASS()
class WEVET_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletBase(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float LifeInterval;

public:
	UPROPERTY(BlueprintAssignable)
	FHitTriggerDelegate HitTriggerDelegate;

	void SetOwners(const TArray<class AActor*> InOwners);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Variable")
	bool bWasHit;

	class UPrimitiveComponent* PrimitiveComponent;

	TArray<class AActor*> IgnoreActors;

	UFUNCTION(BlueprintCallable, Category=Bullet)
	void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
