#pragma once
#include "Engine/EngineTypes.h"
#include "Animation/AnimSequence.h"
#include "CharacterComboInfo.generated.h"


// @NOTE
// 接近戦のコンボ構造体
USTRUCT(BlueprintType)
struct WEVET_API FCharacterComboInfo
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimSequence* AnimSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float BlendInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float BlendOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName SlotNodeName;

	bool bIsValid;
	bool bInfrictResult;

public:
	FCharacterComboInfo()
	{
		bIsValid = false;
		bInfrictResult = false;
		PlayRate = 1.0f;
		BlendInTime = 0.0f;
		BlendOutTime = 0.1f;
		SlotNodeName = TEXT("FullBody");
		AnimSequence = nullptr;
	}

	FORCEINLINE bool operator==(const FCharacterComboInfo& Other) const
	{
		return AnimSequence == Other.AnimSequence;
	}

	void Initialize()
	{
		bIsValid = true;
		bInfrictResult = false;
	}

	void UnInitialize()
	{
		bIsValid = false;
	}

	void SetInfrictResult(const bool NewInfrictResult)
	{
		bInfrictResult = NewInfrictResult;
	}
};

