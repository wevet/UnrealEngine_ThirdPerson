// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "BulletBase.h"
#include "Classes/Animation/AnimMontage.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "WeaponBase.generated.h"

class AMockCharacter;

UCLASS(ABSTRACT)
class WEVET_API AWeaponBase : public AActor, public IWeapon
{
	GENERATED_BODY()

public:
	AWeaponBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	FName MuzzleSocketName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	int32 NeededAmmo;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	bool Visible;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	USoundBase* FireSoundAsset;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	USoundBase* FireImpactSoundAsset;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	UAnimMontage* FireAnimMontageAsset;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	UAnimMontage* ReloadAnimMontageAsset;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	UParticleSystem* MuzzleFlashEmitterTemplate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	UParticleSystem* ImpactMetalEmitterTemplate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	TSubclassOf<class ABulletBase> BulletsBP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Valiable")
	class AMockCharacter* CharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* WidgetComponent;

	USceneComponent* SceneComponent;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWeaponBase|Valiable")
	float BulletDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWeaponBase|Valiable")
	float ReloadDuration;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void SetFireSoundAsset(USoundBase* FireSoundAsset);
	virtual void SetFireAnimMontageAsset(UAnimMontage* FireAnimMontageAsset);
	virtual void SetReloadAnimMontageAsset(UAnimMontage* ReloadAnimMontageAsset);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Valiable")
	virtual void OnEquip(bool Equip);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Valiable")
	virtual void SetPickable(bool Pick);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Valiable")
	virtual void SetReload(bool Reload);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWeaponBase|Valiable")
	FWeaponItemInfo WeaponItemInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|ReadOnlyValiable")
	bool Equip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|ReadOnlyValiable")
	bool CanFired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|ReadOnlyValiable")
	bool CanPick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AWeaponBase|ReadOnlyValiable")
	bool IsReload;

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

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Component")
	virtual	void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|Component")
	virtual	void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	virtual void OnFirePressedInternal(const FVector RelativeLocation, const FVector ForwardLocation, float ForwardOffset);

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	virtual void OnFireReleaseInternal();

	UFUNCTION(BlueprintCallable, Category = "AWeaponBase|API")
	virtual void OnReloadInternal();
	virtual void OnReloadActionInternal();

	FName GetMuzzleSocket() const 
	{
		return this->MuzzleSocketName; 
	}
	
	USkeletalMeshComponent* GetSkeletalMeshComponent() const 
	{
		return this->SkeletalMeshComponent; 
	}

	USphereComponent* GetSphereComponent() const
	{
		return this->SphereComponent;
	}

	USoundBase* GetFireSoundAsset() const 
	{
		return this->FireSoundAsset;  
	}

	USoundBase* GetFireImpactSoundAsset() const
	{
		return this->FireImpactSoundAsset;
	}

	UAnimMontage* GetFireAnimMontageAsset() const 
	{
		return this->FireAnimMontageAsset; 
	}

	UAnimMontage* GetReloadAnimMontageAsset() const 
	{
		return this->ReloadAnimMontageAsset; 
	}

	UWidgetComponent* GetWidgetComponent() const
	{
		return this->WidgetComponent;
	}

	bool GetVisibility() const
	{
		return this->Visible;
	}

};

