// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
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
	UAnimMontage* FireAnimMontageAsset;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Valiable")
	UAnimMontage* ReloadAnimMontageAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Valiable")
	class AMockCharacter* CharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* WidgetComponent;

	USceneComponent* SceneComponent;
	

public:

	virtual void Tick(float DeltaTime) override;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWeaponBase|Valiable")
	float BulletDuration;

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

