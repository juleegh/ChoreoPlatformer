// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCell.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DanceUtilsFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class ETempoAccuracy : uint8
{
	Bad,
	Great,
	Perfect,
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	OneTempoBlock,
	Fruit,
};

USTRUCT(BlueprintType)
struct FTileInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bHitElement;
	UPROPERTY(BlueprintReadOnly)
	class AContextualElement* HitElement;
	UPROPERTY(BlueprintReadOnly)
	ETempoTile TileType;
	UPROPERTY(BlueprintReadOnly)
	float TargetTempo = 1;
	UPROPERTY(BlueprintReadOnly)
	bool bForcesDirection;
	UPROPERTY(BlueprintReadOnly)
	FVector ForcedDirection;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag Section;
};

UCLASS()
class CHOREOPLATFORMER_API UDanceUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetTargetTempo(ETempoTile TileType);
	static FVector GetTransformedPosition(FVector Origin, FVector Direction);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ETempoAccuracy GetTempoResult(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetAcceptanceRate() { return 0.2; }
	static int GetDamageCooldown() { return 5; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetPerfectAcceptanceRate() { return 0.1; }
	static bool PositionsAreEqual(FVector pos1, FVector pos2);
	static float GetHealthDelta(ETempoAccuracy result);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FTileInfo CheckPosition(class AActor* ToIgnore, FVector Start);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static bool IsAdjacentToPlayer(class AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static class UDancerHealthComponent* GetDancerHealthComponent(UWorld* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static class UInventoryComponent* GetInventoryComponent(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static class ATilemapLevelManager* GetTilemapLevelManager(UWorld* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static class USongTempoComponent* GetSongTempoComponent(AActor* WorldContextActor);
};
