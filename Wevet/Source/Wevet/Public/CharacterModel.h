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

	UFUNCTION(BlueprintCallable, Category = "API")
	int32 GetMaxHealth() const { return this->MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "API")
	int32 GetCurrentHealth() const { return this->CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "API")
	int32 GetAttach() const { return this->Attack; }

	UFUNCTION(BlueprintCallable, Category = "API")
	int32 GetDefence() const { return this->Defence; }

	UFUNCTION(BlueprintCallable, Category = "API")
	int32 GetWisdom() const { return this->Wisdom; }

	UFUNCTION(BlueprintCallable, Category = "API")
	virtual void SetCurrentHealthValue(int32 Health);

	UFUNCTION(BlueprintCallable, Category = "API")
	virtual void SetMaxHealthValue(int32 Health);

	UFUNCTION(BlueprintCallable, Category = "API")
	virtual void SetAttackValue(int32 Attack);

	UFUNCTION(BlueprintCallable, Category = "API")
	virtual void SetDefenceValue(int32 Defence);

	UFUNCTION(BlueprintCallable, Category = "API")
	virtual void SetWisdomValue(int32 Wisdom);

	UFUNCTION(BlueprintCallable, Category = "API")
	float GetHealthToWidget() const
	{
		return (float)this->CurrentHealth / (float)this->MaxHealth;
	}

protected:

	UPROPERTY(EditAnywhere, Category = "Variable")
	int32 CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Variable")
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Variable")
	int32 Attack;

	UPROPERTY(EditAnywhere, Category = "Variable")
	int32 Defence;

	UPROPERTY(EditAnywhere, Category = "Variable")
	int32 Wisdom;
};
