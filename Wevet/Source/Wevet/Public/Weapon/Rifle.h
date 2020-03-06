// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/AbstractWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API ARifle : public AAbstractWeapon
{
	GENERATED_BODY()
	
public:
	ARifle(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;

public:
	virtual bool CanMeleeStrike_Implementation() const override;
	virtual void DoReload_Implementation() override;

public:
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }
	FORCEINLINE class USphereComponent* GetSphereComponent() const { return SphereComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	virtual void OnFirePressInternal() override;
	virtual void OnReloadInternal() override;
	virtual FTransform GetMuzzleTransform() const override;

	virtual void BeginOverlapRecieve(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual	void EndOverlapRecieve(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
};
