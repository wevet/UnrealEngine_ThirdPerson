// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MockCharacter.h"
#include "MockPlayerCameraManager.h"
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
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerController|Variable")
	TSubclassOf<class UUMGManager> UMGManagerTemplate;

	UUMGManager* GetPlayerHUD() const;

private:
	class AMockCharacter* Character;
	class AMockPlayerCameraManager* Manager;
	class UUMGManager* UMGManager;

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void OnAlive();

	UFUNCTION()
	void OnKill(AActor* InActor);

public:
	FVector GetCameraRelativeLocation() const
	{
		return Manager ? Manager->GetTransformComponent()->GetComponentLocation() : FVector::ZeroVector;
	}

	FVector GetCameraForwardVector() const
	{
		return Manager ? Manager->GetTransformComponent()->GetForwardVector() : FVector::ZeroVector;
	}
};
