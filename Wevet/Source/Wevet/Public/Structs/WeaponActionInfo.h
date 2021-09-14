#pragma once

#include "Engine/EngineTypes.h"
#include "WevetTypes.h"
#include "Animation/AnimMontage.h"
#include "WeaponActionInfo.generated.h"

USTRUCT(BlueprintType)
struct WEVET_API FAnimSequenceInfo
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimSequence* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float BlendInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float BlendOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName SlotNodeName;

	FAnimSequenceInfo()
	{
		Animation = nullptr;
		PlayRate = 1.0f;
		BlendInTime = 0.2f;
		BlendOutTime = 0.2f;
		SlotNodeName = NAME_None;
	}
};

USTRUCT(BlueprintType)
struct WEVET_API FWeaponActionInfo
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* UnEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FAnimSequenceInfo HitDamageSequence;


	FWeaponActionInfo()
	{
		FireMontage = nullptr;
		ReloadMontage  = nullptr;
		EquipMontage   = nullptr;
		UnEquipMontage = nullptr;
		WeaponItemType = EWeaponItemType::None;
	}

};


