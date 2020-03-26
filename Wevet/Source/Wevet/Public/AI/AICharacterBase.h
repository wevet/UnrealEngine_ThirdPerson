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
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnSensingComponent* PawnSensingComponent;

public:
#pragma region Combat
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;	
#pragma endregion

#pragma region AIPawnOwner
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool IsSeeTarget() const;
	virtual bool IsSeeTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool IsHearTarget() const;
	virtual bool IsHearTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	float GetAttackTraceForwardDistance() const;
	virtual float GetAttackTraceForwardDistance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	float GetAttackTraceLongDistance() const;
	virtual float GetAttackTraceLongDistance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	float GetAttackTraceMiddleDistance() const;
	virtual float GetAttackTraceMiddleDistance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	AActor* GetTarget();
	virtual AActor* GetTarget_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	void StateChange(const EAIActionState NewAIActionState);
	virtual void StateChange_Implementation(const EAIActionState NewAIActionState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|AIPawnOwner")
	bool CanMeleeStrike() const;
	virtual bool CanMeleeStrike_Implementation() const override;	
#pragma endregion

public:
	FORCEINLINE class UPawnSensingComponent* GetPawnSensingComponent() const 
	{
		return PawnSensingComponent; 
	}

	UFUNCTION(BlueprintCallable, Category = "AICharacterBase|Variable")
	bool HasEnemyFound() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	TSubclassOf<class UAIHealthController> UIHealthControllerTemplate;
	class UAIHealthController* UIHealthController;

	virtual void InitializePosses();

	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;
	
	virtual void MainLoop(float DeltaTime);	
	virtual void SetSeeTargetActor(ACharacterBase* const NewCharacter);	
	virtual void SetHearTargetActor(AActor* const OtherActor);

protected:
	class ACharacterBase* TargetCharacter;
	class AAIControllerBase* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	TSubclassOf<class AAbstractWeapon> WeaponTemplate;

	UFUNCTION(BlueprintCallable, Category = "AICharacterBase|PawnSensing")
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);

	UFUNCTION(BlueprintCallable, Category = "AICharacterBase|PawnSensing")
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume);

	/* Attack Trace ForwardDistance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	float AttackTraceForwardDistance;

	/* Attack Trace Middle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	float AttackTraceMiddleDistance;

	/* Attack Trace Long */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	float AttackTraceLongDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	float BulletDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Widget")
	FVector2D WidgetViewPortOffset;

	/* Last bullet action after interval */
	float BulletInterval;

	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Last time we attacked something */
	float LastMeleeAttackTime;

	/* Time-out value to clear the sensed position of the player. */
	/* Should be higher than Sense interval in the PawnSense component not never miss sense ticks.  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AICharacterBase|Variable")
	float SenseTimeOut;

	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	bool bSeeTarget;
	bool bHearTarget;

	virtual void ForceSprint();
	virtual void UnForceSprint();

protected:
	/* Cache rendering */
	bool bWasVisibility;

	virtual void CreateHealthController();
};
