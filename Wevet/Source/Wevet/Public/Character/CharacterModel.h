// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetAttack() const { return Attack; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetDefence() const { return Defence; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetWisdom() const { return Wisdom; }

	void SetHealth(const int32 NewHealth);
	void SetMaxHealth(const int32 NewMaxHealth);
	void SetAttack(const int32 NewAttack);
	void SetDefence(const int32 NewDefence);
	void SetWisdom(const int32 NewWisdom);
	void TakeDamage(const int32 InDamage);

	float GetHealthToWidget() const;
	bool IsHealthHalf() const;
	bool IsHealthQuarter() const;
	bool IsEmptyHealth() const;

	void Die();
	bool IsDie() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Defence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Wisdom;
	bool bDie;
};
