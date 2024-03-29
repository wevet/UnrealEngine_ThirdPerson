// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Pawn.h"
#include "WevetTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WevetBlueprintFunctionLibrary.generated.h"

class UUserWidget;
class UWorld;
class UPrimitiveComponent;
class UMeshComponent;
class AWayPointBase;

UCLASS()
class WEVET_API UWevetBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Loading)
	static void PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime);

	UFUNCTION(BlueprintCallable, Category = Loading)
	static void StopLoadingScreen();

	UFUNCTION(BlueprintPure, Category = Lib)
	static bool IsInEditor();

	UFUNCTION(BlueprintPure, Category = Lib)
	static void CreateDynamicMaterialInstance(UPrimitiveComponent* PrimitiveComponent, TArray<UMaterialInstanceDynamic*>& OutMaterialArray);

	template<typename T>
	static FORCEINLINE TSubclassOf<T> LoadWidgetTemplate(const FString Path)
	{
		FString BasePath("/Game/Blueprints/Widgets/");
		BasePath.Append(Path);
		return TSoftClassPtr<T>(FSoftObjectPath(*BasePath)).LoadSynchronous();
	}

	template<typename T>
	static FORCEINLINE TSubclassOf<T> LoadBlueprintTemplate(const FString Path)
	{
		FString BasePath("/Game/Blueprints/");
		BasePath.Append(Path);
		return TSoftClassPtr<T>(FSoftObjectPath(*BasePath)).LoadSynchronous();
	}

	template<typename T>
	static FORCEINLINE T* SpawnActorDeferred(AActor* PlayerActor, UClass* ItemClass, const FTransform InTransform, AActor* InOwner)
	{
		return PlayerActor->GetWorld()->SpawnActorDeferred<T>(ItemClass, InTransform, InOwner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	}

	template<typename T>
	static FORCEINLINE void SpawnActorDeferred(AActor* PlayerActor, UClass* ItemClass, const FTransform InTransform, AActor* InOwner, TFunction<void(T* Context)> Callback)
	{
		T* SpawningObject = PlayerActor->GetWorld()->SpawnActorDeferred<T>(ItemClass, InTransform, InOwner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Callback(SpawningObject);
		SpawningObject->FinishSpawning(InTransform);
	}

	template<typename T>
	static FORCEINLINE T Select(const bool InType, T A, T B)
	{
		return InType ? A : B;
	}

	UFUNCTION(BlueprintPure, Category = Lib)
	static void OrderByDistance(AActor* PlayerActor, TArray<AActor*>InArray, TArray<AActor*>& OutArray);

	UFUNCTION(BlueprintPure, Category = Lib)
	static AActor* CloneActor(AActor* InputActor);

	UFUNCTION(BlueprintPure, Category = Lib)
	static float GetMeanValue(TArray<float> Values);

	UFUNCTION(BlueprintPure, Category = Lib)
	static void CircleSpawnPoints(const int32 InSpawnCount, const float InRadius, const FVector InRelativeLocation, TArray<FVector>& OutPointArray);

	UFUNCTION(BlueprintCallable, Category = Lib)
	static void WorldPawnIterator(APawn* Owner, const float InDistance, TArray<class APawn*>& OutPawnArray);

	UFUNCTION(BlueprintCallable, Category = Lib)
	static void GetWorldWayPointsArray(AActor* Owner, const float InDistance, TArray<class AWayPointBase*>& OutWayPointArray);

	UFUNCTION(BlueprintCallable, Category = Lib)
	static void DrawDebugString(AActor* const Owner, const FString LogString, FLinearColor DebugColor = FLinearColor::Blue, float Duration = 2.f);

	UFUNCTION(BlueprintCallable, Category = Lib)
	static const bool IsNetworked(AActor* const Owner);

	UFUNCTION(BlueprintCallable, Category = Lib)
	static void SetDepthValue(const ECustomDepthType InCustomDepthType, UMeshComponent* MeshComponent);

	UFUNCTION(BlueprintPure, Category = Lib)
	static TArray<FLinearColor> GetInverseMatrix(const FTransform& InTransform);

	UFUNCTION(BlueprintCallable, Category = Lib)
	static void PyToCpp(const FString String);

	static const FString NormalizeFileName(const char* String);
	static const FString NormalizeFileName(const FString& String);

};
