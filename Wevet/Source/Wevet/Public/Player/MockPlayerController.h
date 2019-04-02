// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MockCharacter.h"
#include "UMG/UMGManager.h"
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
	virtual void Possess(APawn* Pawn) override;
	virtual void UnPossess() override;

protected:
	virtual void Initializer();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMockPlayerController|Variable")
	TSubclassOf<class UUMGManager> UMGManagerClass;

	UUMGManager* GetPlayerHUD() const;

private:
	class AMockCharacter* CharacterOwner;
	class UUMGManager* UMGManager;

};
