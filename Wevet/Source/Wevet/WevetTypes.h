#pragma once
#include "Engine/EngineTypes.h"
#include "WevetTypes.generated.h"

/** List of versions, native code will handle fixups for any old versions */
namespace ESaveGameVersion
{
	enum type
	{
		// Initial version
		Initial,
		// Added Inventory
		AddedInventory,
		// Added ItemData to store count/level
		AddedItemData,
		// -----<new versions must be added before this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
}

UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	None  UMETA(DisplayName = "None"),
	Observation  UMETA(DisplayName = "Observation"),
	Attack  UMETA(DisplayName = "Attack"),
	ConditionCheck UMETA(DisplayName = "ConditionCheck"),
};

UENUM()
enum class EBattlePhaseType : uint8
{
	Normal  UMETA(DisplayName = "Normal"),
	Warning UMETA(DisplayName = "Warning"),
	Alert   UMETA(DisplayName = "Alert"),
};

UENUM()
enum class EBotBehaviorType : uint8
{
	Passive,
	Patrolling,
};

// UI ManagerLayer
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
	Knife  UMETA(DisplayName = "Knife"),
	Bomb   UMETA(DisplayName = "Bomb"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None   UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
	Health UMETA(DisplayName = "Health"),
	Ammos  UMETA(DisplayName = "Ammos"),
};
