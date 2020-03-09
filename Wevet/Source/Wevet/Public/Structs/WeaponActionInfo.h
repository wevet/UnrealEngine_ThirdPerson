#pragma once
#include "Engine/EngineTypes.h"
#include "WevetTypes.h"
#include "Animation/AnimMontage.h"
#include "WeaponActionInfo.generated.h"


USTRUCT(BlueprintType)
struct WEVET_API FWeaponActionInfo
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* UnEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* HitDamageMontage;	

	FWeaponActionInfo()
	{
		FireMontage = nullptr;
		ReloadMontage  = nullptr;
		EquipMontage   = nullptr;
		UnEquipMontage = nullptr;
		HitDamageMontage = nullptr;
		WeaponItemType = EWeaponItemType::None;
	}

};


