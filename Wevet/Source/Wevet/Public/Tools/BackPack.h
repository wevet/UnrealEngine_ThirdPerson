// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "BackPack.generated.h"

class ACharacterBase;
class AAbstractWeapon;

UCLASS()
class WEVET_API ABackPack : public AActor
{
	GENERATED_BODY()
	
public:	
	ABackPack(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BackPack|NativeEvent")
	void BP_OnSimulatePhysics(const bool bWasOwnerDeath);


public:
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);

	void StoreWeapon(AAbstractWeapon* InWeapon, bool &OutAttachSuccess);

	void StartSimulatePhysics();
	void StopSimulatePhysics();

};
