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
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;


public:
#pragma region Interface
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;	
	virtual void OnActionStateChange_Implementation() override;
	virtual void DoSightReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew) override;
	virtual void DoHearReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew) override;
	virtual void DoPredictionReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus) override;
	virtual void DoDamageReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus) override;
	virtual bool CanStrike_Implementation() const override;
	virtual void DoFirePressed_Implementation() override;
#pragma endregion


protected:
	virtual void DoWhileALSMovementMode() override;
	virtual void EquipmentActionMontage() override;
	virtual void ReleaseAllWeaponInventory() override;
	virtual void ReleaseAllItemInventory() override;

public:
	virtual void StartRagdollAction() override;
	virtual void RagdollToWakeUpAction() override;

public:
	void RemoveSearchNodeGenerator();

protected:
	void CreateHealthController();
	void DestroyHealthController();
	void CreateSearchNodeGenerator(const FVector SearchOriginLocation);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class ASearchNodeGenerator> NodeHolderTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class UAIHealthController> UIHealthControllerTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	FVector2D WidgetViewPortOffset;

	class AAIControllerBase* AIController;
	class UAIHealthController* UIHealthController;
};
