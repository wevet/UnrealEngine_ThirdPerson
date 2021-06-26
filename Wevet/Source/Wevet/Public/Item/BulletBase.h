// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageTypeInstigator.h"
#include "Components/PrimitiveComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "BulletBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHitTriggerDelegate, AActor* const, OtherActor, FHitResult const, SweepResult);

UCLASS()
class WEVET_API ABulletBase : public AActor, public IDamageTypeInstigator
{
	GENERATED_BODY()
	
public:	
	ABulletBase(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bullet|DamageTypeInstigator")
	EGiveDamageType GetGiveDamageType() const;
	virtual EGiveDamageType GetGiveDamageType_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|Variable")
	float LifeInterval;

	bool bWasHitResult;
	bool bWasOverlapResult;

public:
	UPROPERTY(BlueprintAssignable)
	FHitTriggerDelegate HitTriggerDelegate;

	void SetOwners(const TArray<class AActor*>& InOwners);

	void VisibleEmitter(const bool InVisible);

protected:
	class UPrimitiveComponent* PrimitiveComponent;
	class UParticleSystemComponent* ParticleComponent;

	TArray<class AActor*> IgnoreActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet|Variable")
	class UParticleSystem* ImpactWaterEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet|Variable")
	class UParticleSystem* ImpactBloodEmitterTemplate;

	UFUNCTION()
	void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
