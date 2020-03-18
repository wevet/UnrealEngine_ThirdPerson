// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/WeaponInstigator.h"
#include "Interface/InteractionInstigator.h"
#include "WevetTypes.h"
#include "AbstractWeapon.generated.h"

class ACharacterBase;
class ABulletBase;

UCLASS(ABSTRACT)
class WEVET_API AAbstractWeapon : public AActor, public IWeaponInstigator, public IInteractionInstigator
{
	GENERATED_BODY()
	
public:	
	AAbstractWeapon(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;

public:
#pragma region WeaponInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFirePressed();
	virtual void DoFirePressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFireRelease();
	virtual void DoFireRelease_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReload();
	virtual void DoReload_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReplenishment(const FWeaponItemInfo& RefWeaponItemInfo);
	virtual void DoReplenishment_Implementation(const FWeaponItemInfo& RefWeaponItemInfo) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	bool CanMeleeStrike() const;
	virtual bool CanMeleeStrike_Implementation() const override;
#pragma endregion

#pragma region InteractionInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	void Take(ACharacterBase* NewCharacter);
	virtual void Take_Implementation(ACharacterBase* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	void Release(ACharacterBase* NewCharacter);
	virtual void Release_Implementation(ACharacterBase* NewCharacter) override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	EItemType GetItemType() const;
	virtual EItemType GetItemType_Implementation() const override;
#pragma endregion

public:
	virtual void Initialize(ACharacterBase* const NewCharacterOwner);
	virtual void ResetCharacterOwner();

	FORCEINLINE class USceneComponent* GetSceneComponent() const { return SceneComponent; }

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	ACharacterBase* GetPointer() const;

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	APawn* GetPawnOwner() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FWeaponItemInfo WeaponItemInfo;

	void OnReloadInternal();

protected:
	TWeakObjectPtr<ACharacterBase> CharacterPtr;
	TArray<class AActor*> IgnoreActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void CheckIfValid(AActor* OtherActor, bool& bCanDamageResult);

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void HitReceive(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void BeginOverlapRecieve(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual	void EndOverlapRecieve(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void TakeDamageOuter(const FHitResult& HitResult, const float AdditionalDamage);

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent);

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void OnFirePressInternal();

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void OnFireReleaseInternal();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	int32 NeededAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float BulletDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float ReloadDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bFirePressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* FlashEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* ImpactEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	TSubclassOf<class ABulletBase> BulletsTemplate;

	FTimerHandle ReloadTimerHandle;
	virtual void PrepareDestroy();

public:
	virtual FTransform GetMuzzleTransform() const;
	virtual void SetEquip(const bool InEquip);
	virtual void ReloadSound();
	void SetReload(const bool InReload);

	bool WasEquip() const;
	bool WasReload() const;

	bool WasSameWeaponType(EWeaponItemType InWeaponItemType) const;
	void CopyWeaponItemInfo(const FWeaponItemInfo RefWeaponItemInfo);
};
