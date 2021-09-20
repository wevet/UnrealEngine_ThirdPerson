// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageTypeInstigator.h"
#include "WeaponTriggerBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "BulletBase.generated.h"


UCLASS()
class WEVET_API ABulletBase : public AWeaponTriggerBase, public IDamageTypeInstigator
{
	GENERATED_BODY()
	
public:	
	ABulletBase(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


protected:
	virtual void BeginPlay() override;


protected:
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bullet|DamageTypeInstigator")
	EGiveDamageType GetGiveDamageType() const;
	virtual EGiveDamageType GetGiveDamageType_Implementation() const override;

	void VisibleEmitter(const bool InVisible);


protected:
	class UParticleSystemComponent* ParticleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet|Variable")
	float LifeInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet|Variable")
	class UParticleSystem* ImpactWaterEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet|Variable")
	class UParticleSystem* ImpactBloodEmitterTemplate;

};
