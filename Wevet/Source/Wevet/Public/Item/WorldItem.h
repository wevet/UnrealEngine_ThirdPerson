// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WevetTypes.h"
#include "Interface/InteractionItem.h"
#include "WorldItem.generated.h"

class APawn;
class UPrimitiveComponent;

UCLASS(ABSTRACT)
class WEVET_API AWorldItem : public AActor, public IInteractionItem
{
	GENERATED_BODY()
	
public:	
	AWorldItem(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WorldItem|Interaction")
	void Take(APawn* NewCharacter);
	virtual void Take_Implementation(APawn* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WorldItem|Interaction")
	void Release(APawn* NewCharacter);
	virtual void Release_Implementation(APawn* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WorldItem|Interaction")
	void SpawnToWorld();
	virtual void SpawnToWorld_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WorldItem|Interaction")
	EItemType GetItemType() const;
	virtual EItemType GetItemType_Implementation() const override;
#pragma endregion


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WorldItem|Variable")
	class USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WorldItem|Variable")
	bool bRenderCutomDepthEnable;


protected:
	UFUNCTION()
	virtual void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void MarkRenderStateDirty(UPrimitiveComponent* PrimitiveComponent, const bool InEnable, const ECustomDepthType InDepthType);


	virtual void AddDelegate()
	{
	}

	virtual void RemoveDelegate()
	{
	}

	virtual void OverlapActor(AActor* OtherActor)
	{
	}

	virtual void PrepareDestroy()
	{
	}
};

