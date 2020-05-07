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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnSensingComponent* PawnSensingComponent;

public:
#pragma region Interface
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;	

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
	AActor* GetTarget() const;
	virtual AActor* GetTarget_Implementation() const override;

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
	
	virtual void InitializePosses();

	virtual FVector BulletTraceRelativeLocation() const override
	{
		if (CurrentWeapon.IsValid())
		{
			return CurrentWeapon.Get()->GetMuzzleTransform().GetLocation();
		}
		return Super::BulletTraceRelativeLocation();
	}

	virtual FVector BulletTraceForwardLocation() const override
	{
		if (CurrentWeapon.IsValid())
		{
			return CurrentWeapon.Get()->GetMuzzleTransform().GetRotation().GetForwardVector();
		}
		return Super::BulletTraceForwardLocation();
	}

	virtual void SetSeeTargetActor(ACharacterBase* const NewCharacter);	
	virtual void SetHearTargetActor(AActor* const OtherActor);

	void SetBulletInterval(const float InBulletInterval)
	{
		BulletInterval = InBulletInterval;
	}

	void SetMeleeAttackTimeOut(const float InMeleeAttackTimeOut)
	{
		MeleeAttackTimeOut = InMeleeAttackTimeOut;
	}

	void DoCoverAI();

	FORCEINLINE bool WasMeleeAttacked() const { return (MeleeAttackTimeOut >= ZERO_VALUE); }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	TSubclassOf<class UAIHealthController> UIHealthControllerTemplate;
	class UAIHealthController* UIHealthController;


protected:
	virtual void MainLoop(float DeltaTime);

	UFUNCTION()
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);

	UFUNCTION()
	virtual	void OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume);

	virtual void ForceSprint();
	virtual void UnForceSprint();
	virtual void CreateHealthController();

protected:
	class ACharacterBase* TargetCharacter;
	class AAIControllerBase* AIController;

	/* Attack Trace ForwardDistance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float AttackTraceForwardDistance;

	/* Attack Trace Middle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float AttackTraceMiddleDistance;

	/* Attack Trace Long */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float AttackTraceLongDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float BulletDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float SenseTimeOut;

	/* Time-out value to melee attack time. */
	float MeleeAttackTimeOut;

	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	bool bSeeTarget;
	bool bHearTarget;

protected:
	/* Cache rendering */
	bool bWasVisibility;
};
