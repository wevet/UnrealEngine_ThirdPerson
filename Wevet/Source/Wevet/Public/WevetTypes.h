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

namespace Wevet
{
	enum BattlePhase
	{
		Normal,
		Warning,
		Alert,
	};

}

UENUM()
enum class ECustomDepthType : uint8
{
	None = 0,
	Environment = 1,
	Weapon = 10,
	Item = 20,
	Pawn = 30,
	//Max = 256,
};

// 
UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Search UMETA(DisplayName = "Search"),
	Combat UMETA(DisplayName = "Combat"),
};

// UI ManagerLayer
UENUM()
enum class EUMGLayerType : int32
{
	None = -1,
	WorldScreen = 0,
	Main = 1,
	Overlay = 100,
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
	Weapon UMETA(DisplayName = "Weapon"), // AAbstractWeapon
	Health UMETA(DisplayName = "Health"), 
	Ammos  UMETA(DisplayName = "Ammos"),  // AAbstractItem
};

UENUM(BlueprintType)
enum class EGiveDamageType : uint8
{
	None   UMETA(DisplayName = "None"),
	Shoot  UMETA(DisplayName = "Shoot"),
	Melee  UMETA(DisplayName = "Melee"),
};

