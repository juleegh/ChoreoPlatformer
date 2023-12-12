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
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static bool IsAdjacentToPlayer(class AActor* WorldContextActor, int TilesAway);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ETempoAccuracy GetTempoResult(float Distance);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetAcceptanceRate() { return 0.25; }
	static int GetDamageCooldown() { return 5; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetPerfectAcceptanceRate() { return 0.15; }
	static bool PositionsAreEqual(FVector pos1, FVector pos2);
	static float GetHealthDelta(ETempoAccuracy result);
	UFUNCTION(BlueprintCallable)
	static void ToggleHighlight(AActor* Target, bool Highlighted);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FTileInfo CheckPosition(TArray<class AActor*> ToIgnore, FVector Start);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<AActor*> GetAdjacent(class AActor* ToIgnore, FVector Position, int Radius);
};
