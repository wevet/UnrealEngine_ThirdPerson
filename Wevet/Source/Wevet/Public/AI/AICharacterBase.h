// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "AI/AIControllerBase.h"
#include "AI/SearchNodeGenerator.h"
#include "AI/BaseInvestigationNode.h"
#include "AICharacterBase.generated.h"

class UAIHealthController;

UCLASS(ABSTRACT)
class WEVET_API AAICharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AAICharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

#pragma region Interface
public:
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;	
	virtual void OnActionStateChange_Implementation() override;
	virtual void DoSightReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew) override;
	virtual void DoHearReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew) override;
	virtual void DoPredictionReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus) override;
	virtual void DoDamageReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus) override;

	// ALS
	virtual void Initializer_Implementation() override;
#pragma endregion

public:
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const 
	{
		return BehaviorTree; 
	}

protected:
	virtual void EquipmentActionMontage() override;

public:
	virtual void StartRagdollAction() override;
	virtual void RagdollToWakeUpAction() override;

protected:
	void CreateHealthController();
	void DestroyHealthController();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class ASearchNodeGenerator> NodeHolderTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class UAIHealthController> UIHealthControllerTemplate;

	class AAIControllerBase* AIController;
	class UAIHealthController* UIHealthController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	FVector2D WidgetViewPortOffset;

protected:
	void CreateSearchNodeGenerator(const FVector SearchOriginLocation);

public:
	void RemoveSearchNodeGenerator();
};
