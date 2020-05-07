// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MockCharacter.h"
#include "Widget/UMGManager.h"
#include "MockPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API AMockPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMockPlayerController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	virtual void Initializer();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerController|Variable")
	TSubclassOf<class UUMGManager> UMGManagerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerController|Variable")
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerController|Variable")
	float ViewPitchMax;

	UUMGManager* GetPlayerHUD() const;

private:
	class AMockCharacter* CharacterOwner;
	class UUMGManager* UMGManager;

};
