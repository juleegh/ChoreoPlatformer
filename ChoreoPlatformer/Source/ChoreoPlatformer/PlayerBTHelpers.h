// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCell.h"
#include "DanceDefinitions.h"
#include "TilemapLevelManager.h"
#include "DancerHealthComponent.h"
#include "LevelProgressComponent.h"
#include "DancerUIComponent.h"
#include "DanceAudioManager.h"
#include "InventoryComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerBTHelpers.generated.h"

UCLASS()
class CHOREOPLATFORMER_API UPlayerBTHelpersFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetPlayerInput(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool CanMove(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsOnTempo(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static AChoreoPlayerController* GetChoreoPlayerController(AActor* Player);
};
