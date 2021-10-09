// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WevetTypes.h"
#include "WeaponTriggerBase.h"
#include "Components/StaticMeshComponent.h"
#include "NakedWeaponTrigger.generated.h"


UCLASS()
class WEVET_API ANakedWeaponTrigger : public AWeaponTriggerBase
{
	GENERATED_BODY()
	
public:	
	ANakedWeaponTrigger(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


protected:
	virtual void BeginPlay() override;


protected:
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	ENakedWeaponTriggerType NakedWeaponTriggerType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	FName AttachBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	float AddtionalDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
	bool bShowMesh;


public:
	ENakedWeaponTriggerType GetNakedWeaponTriggerType() const
	{
		return NakedWeaponTriggerType; 
	}

	//FORCEINLINE float GetAddtionalDamage() const { return AddtionalDamage; }
	//FORCEINLINE FName GetAttachBoneName() const { return AttachBoneName; }


public:
	void NakedActionApply(const bool Enable);
};

