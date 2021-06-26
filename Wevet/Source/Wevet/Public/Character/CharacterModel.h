// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Wevet.h"
#include "CharacterModel.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class WEVET_API UCharacterModel : public UObject
{
	GENERATED_BODY()
	
public:
	UCharacterModel(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE int32 GetAttack() const { return Attack; }
	FORCEINLINE int32 GetDefence() const { return Defence; }
	FORCEINLINE int32 GetWisdom() const { return Wisdom; }
	FORCEINLINE int32 GetKillDamage() const { return MaxHealth; }
	FORCEINLINE float GetHealthToWidget() const { return (float)Health / (float)MaxHealth; }

	FORCEINLINE bool IsFullHealth() const { return Health >= MaxHealth; }
	FORCEINLINE bool CanDie() const { return (Health <= INT_ZERO); }
	FORCEINLINE bool IsDie() const { return (Health <= INT_ZERO) && bDie; }


	FORCEINLINE bool IsHealthHalf() const
	{
		float Value = ((float)MaxHealth * HALF_WEIGHT);
		int32 HalfValue = (int32)Value;
		return Health < HalfValue;
	}

	FORCEINLINE bool IsHealthQuarter() const
	{
		float Value = ((float)MaxHealth * QUART_WEIGHT);
		int32 QuartValue = (int32)Value;
		return Health < QuartValue;
	}

	void SetHealth(const int32 NewHealth);
	void SetMaxHealth(const int32 NewMaxHealth);
	void SetAttack(const int32 NewAttack);
	void SetDefence(const int32 NewDefence);
	void SetWisdom(const int32 NewWisdom);

	void DoTakeDamage(const int32 InDamage);
	void DoRecover(const int32 AddHealth);
	void DoDie();
	void DoAlive();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 Defence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 Wisdom;

	UPROPERTY()
	bool bDie;
};
