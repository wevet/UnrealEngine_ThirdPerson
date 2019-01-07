#pragma once
#include "Engine/EngineTypes.h"
#include "GameFramework/DamageType.h"
#include "STypes.generated.h"


UENUM()
enum class EBotBehaviorType : uint8
{
	Passive,
	Patrolling,
};

UENUM()
enum class EUMGLayerType : int32
{
	None = -1,
	Base = 0,
	Main = 1,
	Num,
};

UENUM(BlueprintType)
enum class EWeaponItemType : uint8
{
	None   UMETA(DisplayName = "None"),
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle  UMETA(DisplayName = "Rifle"),
	Sniper UMETA(DisplayName = "Sniper"),
	Bomb   UMETA(DisplayName = "Bomb"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None   UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
	Health UMETA(DisplayName = "Health"),
};

USTRUCT(BlueprintType)
struct WEVET_API FWeaponItemInfo
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 ClipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName UnEquipSocketName;

	FWeaponItemInfo()
	{
		EquipSocketName = FName(TEXT("Lancer_Root_Socket"));
		Damage = 20.f;
		TraceDistance = 15000.f;
	}

public:
	friend FArchive& operator<<(FArchive& Ar, FWeaponItemInfo& TheStruct)
	{
		Ar << TheStruct.WeaponItemType;
		Ar << TheStruct.WeaponClass;
		Ar << TheStruct.ClipType;
		Ar << TheStruct.CurrentAmmo;
		Ar << TheStruct.MaxAmmo;
		Ar << TheStruct.Damage;
		Ar << TheStruct.Texture;
		Ar << TheStruct.EquipSocketName;
		Ar << TheStruct.UnEquipSocketName;
		return Ar;
	}

public:
	void CopyTo(const FWeaponItemInfo& InWeaponItemInfo)
	{
		UnEquipSocketName = InWeaponItemInfo.UnEquipSocketName;
		EquipSocketName = InWeaponItemInfo.EquipSocketName;
		WeaponItemType = InWeaponItemInfo.WeaponItemType;
		WeaponClass = InWeaponItemInfo.WeaponClass;
		CurrentAmmo = InWeaponItemInfo.CurrentAmmo;
		ClipType = InWeaponItemInfo.ClipType;
		MaxAmmo = InWeaponItemInfo.MaxAmmo;
		Damage = InWeaponItemInfo.Damage;
		Texture = InWeaponItemInfo.Texture;
	}

};


USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	UClass* DamageTypeClass;

	UPROPERTY()
	TWeakObjectPtr<class ACharacterBase> PawnInstigator;

	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	UPROPERTY()
	uint8 DamageEventClassID;

	UPROPERTY()
	bool bKilled;

private:

	UPROPERTY()
	uint8 EnsureReplicationByte;

	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo() 
		: ActualDamage(0),
		DamageTypeClass(nullptr),
		PawnInstigator(nullptr),
		DamageCauser(nullptr),
		DamageEventClassID(0),
		bKilled(false),
		EnsureReplicationByte(0)
	{}

	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == nullptr)
			{
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == nullptr)
			{
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return RadialDamageEvent;

		default:
			if (GeneralDamageEvent.DamageTypeClass == nullptr)
			{
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return GeneralDamageEvent;
		}
	}

	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}
	}

	void EnsureReplication()
	{
		++EnsureReplicationByte;
	}
};