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
	int32 GetMaxHealth() const { return this->MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetCurrentHealth() const { return this->CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetAttach() const { return this->Attack; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetDefence() const { return this->Defence; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	int32 GetWisdom() const { return this->Wisdom; }

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetCurrentHealthValue(int32 Health);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetMaxHealthValue(int32 Health);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetAttackValue(int32 Attack);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetDefenceValue(int32 Defence);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	virtual void SetWisdomValue(int32 Wisdom);

	UFUNCTION(BlueprintCallable, Category = "CharacterCondition")
	float GetHealthToWidget() const
	{
		return (float)this->CurrentHealth / (float)this->MaxHealth;
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
