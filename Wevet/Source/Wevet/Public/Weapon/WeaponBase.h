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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	int32 NeededAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	USoundBase* FireSoundAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	USoundBase* FireImpactSoundAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* FireAnimMontageAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* ReloadAnimMontageAsset;

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

public:

	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const { return this->SkeletalMeshComponent; }
	FORCEINLINE class USphereComponent* GetSphereComponent() const { return this->SphereComponent; }
	FORCEINLINE class UWidgetComponent* GetWidgetComponent() const { return this->WidgetComponent; }

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Variable")
	virtual void OnEquip(const bool Equip);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Variable")
	virtual void SetPickable(const bool Pick);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Variable")
	virtual void SetReload(const bool Reload);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWeaponBase|Variable")
	FWeaponItemInfo WeaponItemInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool Equip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool CanFired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool CanPick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|Variable")
	bool IsReload;

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

	virtual void OnReloadInternal();

	FName GetMuzzleSocket() const { return this->MuzzleSocketName; }

	const FTransform GetMuzzleTransform()
	{ 
		return this->SkeletalMeshComponent->GetSocketTransform(this->GetMuzzleSocket()); 
	}

	virtual void SetFireSoundAsset(USoundBase* FireSoundAsset);
	virtual void SetFireAnimMontageAsset(UAnimMontage* FireAnimMontageAsset);
	virtual void SetReloadAnimMontageAsset(UAnimMontage* ReloadAnimMontageAsset);
	USoundBase* GetFireSoundAsset() const { return this->FireSoundAsset; }
	USoundBase* GetFireImpactSoundAsset() const { return this->FireImpactSoundAsset; }
	UAnimMontage* GetFireAnimMontageAsset() const  { return this->FireAnimMontageAsset;  }
	UAnimMontage* GetReloadAnimMontageAsset() const { return this->ReloadAnimMontageAsset; }

	bool HasMatchTypes(EWeaponItemType InWeaponItemType) const
	{
		return WeaponItemInfo.WeaponItemType == InWeaponItemType;
	}

	virtual void SetCharacterOwner(ACharacterBase* InCharacterOwner);
};

