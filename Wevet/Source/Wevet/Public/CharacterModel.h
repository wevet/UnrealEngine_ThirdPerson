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
	int32 GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetAttach() const { return Attack; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetDefence() const { return Defence; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetWisdom() const { return Wisdom; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetCurrentHealthValue(int32 NewHealth);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetMaxHealthValue(int32 NewHealth);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetAttackValue(int32 NewAttack);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetDefenceValue(int32 NewDefence);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetWisdomValue(int32 NewWisdom);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	float GetHealthToWidget() const
	{
		return (float)CurrentHealth / (float)MaxHealth;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Defence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterCondition")
	int32 Wisdom;
};
