// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Interface/AIPawnOwner.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AICharacterBase.generated.h"

class AAIControllerBase;
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
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnSensingComponent* PawnSensingComponent;

#pragma region Interface
public:
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool IsSeeTarget() const;
	virtual bool IsSeeTarget_Implementation() const override { return bSeeTarget; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool IsHearTarget() const;
	virtual bool IsHearTarget_Implementation() const override { return bHearTarget; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	float GetMeleeDistance() const;
	virtual float GetMeleeDistance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	AActor* GetTarget() const;
	virtual AActor* GetTarget_Implementation() const override { return TargetCharacter; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	void StateChange(const EAIActionState NewAIActionState);
	virtual void StateChange_Implementation(const EAIActionState NewAIActionState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool CanMeleeStrike() const;
	virtual bool CanMeleeStrike_Implementation() const override;	
#pragma endregion

public:
	FORCEINLINE class UPawnSensingComponent* GetPawnSensingComponent() const { return PawnSensingComponent;  }
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE bool WasMeleeAttacked() const { return (MeleeAttackTimeOut >= ZERO_VALUE); }
	FORCEINLINE bool WasAttackInitialized() const { return bAttackInitialized; }

public:
	virtual void InitializePosses();
	virtual void Sprint() override;
	virtual void StopSprint() override;

	virtual void AttackInitialize(const float InInterval, const float InTimeOut);
	virtual void AttackUnInitialize();
	virtual void FindFollowCharacter();
	virtual FVector BulletTraceForwardLocation() const override;

protected:
	virtual void StartRagdollAction() override;
	virtual void RagdollToWakeUpAction() override;
	virtual void EquipmentActionMontage() override;
	virtual void UnEquipmentActionMontage() override;

protected:
	virtual void MainLoop(float DeltaTime);
	virtual void CreateHealthController();
	virtual void DestroyHealthController();

	UFUNCTION()
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);
	virtual void SeePawnRecieveCallback(ACharacterBase* const NewCharacter);

	UFUNCTION()
	virtual	void OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume);
	virtual void HearNoiseRecieveCallback(AActor* const OtherActor, FVector Location = FVector::ZeroVector);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class UAIHealthController> UIHealthControllerTemplate;
	class UAIHealthController* UIHealthController;

	class ACharacterBase* TargetCharacter;
	class AAIControllerBase* AIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float BulletDelay;

	/* Time-out value to clear the sensed position of the player. */
	/* Should be higher than Sense interval in the PawnSense component not never miss sense ticks.  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float SenseTimeOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float HearTimeOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	FVector2D WidgetViewPortOffset;

protected:
	/* Last bullet action after interval */
	float BulletInterval;

	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Last time we attacked something */
	float LastMeleeAttackTime;

	/* Time-out value to melee attack time. */
	float MeleeAttackTimeOut;

	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	bool bSeeTarget;
	bool bHearTarget;
	bool bAttackInitialized;

};
