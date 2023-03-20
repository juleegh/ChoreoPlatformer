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

UCLASS()
class CHOREOPLATFORMER_API UDanceUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static float GetTargetTempo(ETempoTile TileType);
	static FVector GetTransformedPosition(FVector Origin, FVector Direction);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ETempoAccuracy GetTempoResult(float Distance);
	static float GetAcceptanceRate() { return 0.3; }
	static float GetPerfectAcceptanceRate() { return 0.1; }
};
