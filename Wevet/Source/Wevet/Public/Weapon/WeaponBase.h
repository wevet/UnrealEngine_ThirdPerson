// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponControllerExecuter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponBase.generated.h"

class ACharacterBase;
class ABulletBase;

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
	USoundBase* PickupSoundAsset;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;

	USceneComponent* SceneComponent;
	FTimerHandle ReloadTimerHandle;
	TWeakObjectPtr<class ACharacterBase> CharacterOwner;

	virtual void TakeHitDamage(const FHitResult HitResult);
	virtual void PlayBulletEffect(UWorld* const World, const FHitResult HitResult);

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

	UFUNCTION(BlueprintCallable, Category = Components)
	virtual	void BeginOverlapRecieve(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable, Category = Components)
	virtual	void EndOverlapRecieve(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = API)
	virtual void OnFirePressedInternal();

	virtual void OnFireReleaseInternal();
	virtual void OnReloadInternal();
	virtual void Take(ACharacterBase* NewCharacter);
	virtual void Release(ACharacterBase* NewCharacter);
	virtual void Recover(const FWeaponItemInfo RefWeaponItemInfo);

	const FTransform GetMuzzleTransform()
	{
		return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName);
	}

	bool HasMatchTypes(EWeaponItemType InWeaponItemType) const
	{
		return WeaponItemInfo.WeaponItemType == InWeaponItemType;
	}

	virtual void SetCharacterOwner(ACharacterBase* NewCharacter);
	void CopyWeaponItemInfo(const FWeaponItemInfo RefWeaponItemInfo);

protected:
	FTraceDelegate TraceDelegate;
	FTraceHandle LastTraceHandle;

	virtual void AsyncTraceUpdate(const float DeltaTime);
	virtual void OnTraceCompleted(const FTraceHandle& Handle, FTraceDatum& Data);
	virtual void PrepareDestroy();
};
