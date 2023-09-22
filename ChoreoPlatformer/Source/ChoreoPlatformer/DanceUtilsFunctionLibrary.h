// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCell.h"
#include "DanceDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DanceUtilsFunctionLibrary.generated.h"

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
	static float GetAcceptanceRate() { return 0.18; }
	static int GetDamageCooldown() { return 5; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetPerfectAcceptanceRate() { return 0.1; }
	static bool PositionsAreEqual(FVector pos1, FVector pos2);
	static float GetHealthDelta(ETempoAccuracy result);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FTileInfo CheckPosition(TArray<class AActor*> ToIgnore, FVector Start);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static class ADanceCharacter* GetDanceCharacter(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static bool IsAdjacentToPlayer(class AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static class UDancerHealthComponent* GetDancerHealthComponent(UWorld* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static class UInventoryComponent* GetInventoryComponent(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static class ASectionLevelManager* GetSectionLevelManager(UWorld* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static class ATilemapLevelManager* GetTilemapLevelManager(UWorld* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static class USongTempoComponent* GetSongTempoComponent(AActor* WorldContextActor);
};
