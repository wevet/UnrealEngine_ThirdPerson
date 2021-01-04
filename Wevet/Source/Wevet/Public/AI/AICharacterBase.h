// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "AI/AIControllerBase.h"
#include "AI/SearchNodeGenerator.h"
#include "AI/BaseInvestigationNode.h"

#include "Interface/AIPawnOwner.h"

#include "AICharacterBase.generated.h"

class UAIHealthController;

UCLASS(ABSTRACT)
class WEVET_API AAICharacterBase : public ACharacterBase, public IAIPawnOwner
{
	GENERATED_BODY()

public:
	AAICharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	float GetMeleeDistance() const;
	virtual float GetMeleeDistance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	AActor* GetTarget() const;
	virtual AActor* GetTarget_Implementation() const override { return TargetCharacter; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	void CombatStateChange(const EAICombatState NewAICombatState);
	virtual void CombatStateChange_Implementation(const EAICombatState NewAICombatState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	void ActionStateChange(const EAIActionState NewAIActionState);
	virtual void ActionStateChange_Implementation(const EAIActionState NewAIActionState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool CanMeleeStrike() const;
	virtual bool CanMeleeStrike_Implementation() const override;	
#pragma endregion

public:
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const 
	{
		return BehaviorTree; 
	}

	FORCEINLINE bool WasMeleeAttacked() const 
	{
		return (MeleeAttackTimeOut >= ZERO_VALUE); 
	}

	FORCEINLINE bool WasAttackInitialized() const 
	{
		return bAttackInitialized; 
	}

public:
	virtual void InitializePosses();
	virtual void Sprint() override;
	virtual void StopSprint() override;

	virtual FVector BulletTraceForwardLocation() const override;

	virtual void OnSightStimulus(AActor* const Actor, const FAIStimulus InStimulus, const bool InWasDeadCrew);
	virtual void OnHearStimulus(AActor* const Actor, const FAIStimulus InStimulus, const bool InWasDeadCrew);
	virtual void OnPredictionStimulus(AActor* const Actor, const FAIStimulus InStimulus);

protected:
	virtual void StartRagdollAction() override;
	virtual void RagdollToWakeUpAction() override;
	virtual void EquipmentActionMontage() override;
	virtual void UnEquipmentActionMontage() override;

protected:
	virtual void CreateHealthController();
	virtual void DestroyHealthController();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class ASearchNodeGenerator> NodeHolderTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class UAIHealthController> UIHealthControllerTemplate;
	class UAIHealthController* UIHealthController;

	class ACharacterBase* TargetCharacter;
	class AAIControllerBase* AIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float BulletDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	FVector2D WidgetViewPortOffset;

protected:
	/* Last bullet action after interval */
	float BulletInterval;

	/* Already Attack Initialized ? */
	bool bAttackInitialized;

public:
	virtual void AttackInitialize(const float InInterval, const float InTimeOut);
	virtual void AttackUnInitialize();

protected:
	void UpdateSearchNodeHolder(const FVector SearchOriginLocation);

public:
	void RemoveSearchNodeGenerator();
};
