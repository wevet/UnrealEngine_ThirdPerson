// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WevetGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API UWevetGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UWevetGameInstance();
	virtual void Init() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = Save)
	bool LoadOrCreateSaveGame();

	UFUNCTION(BlueprintCallable, Category = Save)
	void ResetSaveGame(bool& bClearSuccess);

protected:
	UFUNCTION(BlueprintCallable, Category = Save)
	void SetSavingEnabled(bool bEnabled);

	UPROPERTY(BlueprintReadWrite, Category = Save)
	FString SaveSlot;

	UPROPERTY(BlueprintReadWrite, Category = Save)
	int32 SaveUserIndex;

	UFUNCTION(BlueprintCallable, Category = Save)
	bool WriteSaveGame();

	/** Rather it will attempt to actually save to disk */
	UPROPERTY()
	bool bSavingEnabled;

public:
	static UWevetGameInstance* GetInstance();

private:
	static UWevetGameInstance* Instance;
};
