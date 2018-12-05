// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponControllerExecuter.h"
#include "BulletBase.h"
#include "Classes/Animation/AnimMontage.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "WeaponBase.generated.h"

class ACharacterBase;

UCLASS(ABSTRACT)
class WEVET_API AWeaponBase : public AActor, public IWeaponControllerExecuter
{
	GENERATED_BODY()

public:
	AWeaponBase(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 NeededAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	USoundBase* FireSoundAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	USoundBase* FireImpactSoundAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	UParticleSystem* MuzzleFlashEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	UParticleSystem* ImpactMetalEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	TSubclassOf<class ABulletBase> BulletsBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float BulletDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float ReloadDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	class ACharacterBase* CharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;

	USceneComponent* SceneComponent;
	FTimerHandle ReloadTimerHandle;
	UClass* OwnerClass;

public:
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const 
	{ 
		return SkeletalMeshComponent; 
	}

	FORCEINLINE class USphereComponent* GetSphereComponent() const 
	{
		return SphereComponent; 
	}
	
	FORCEINLINE class UWidgetComponent* GetWidgetComponent() const 
	{
		return WidgetComponent; 
	}

	virtual void SetEquip(const bool Equip);
	virtual void SetReload(const bool Reload);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AWeaponBase|Variable")
	FWeaponItemInfo WeaponItemInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool bEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool bFired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool bReload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool bEmpty;

#pragma region interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AWeaponBase|Interface")
	void OnFirePress();
	virtual void OnFirePress_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AWeaponBase|Interface")
	void OnFireRelease();
	virtual void OnFireRelease_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AWeaponBase|Interface")
	void OnReloading();
	virtual void OnReloading_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AWeaponBase|Interface")
	void OffVisible();
	virtual void OffVisible_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AWeaponBase|Interface")
	void OnVisible();
	virtual void OnVisible_Implementation() override;
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Component")
	virtual	void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Component")
	virtual	void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	virtual void OnFirePressedInternal();

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	virtual void OnFireReleaseInternal();

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	virtual void OnReloadInternal();

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	const FTransform GetMuzzleTransform()
	{ 
		return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName);
	}

	bool HasMatchTypes(EWeaponItemType InWeaponItemType) const
	{
		return WeaponItemInfo.WeaponItemType == InWeaponItemType;
	}

	virtual void SetCharacterOwner(ACharacterBase* InCharacterOwner);

	void CopyTo(const FWeaponItemInfo& InWeaponItemInfo);
};

