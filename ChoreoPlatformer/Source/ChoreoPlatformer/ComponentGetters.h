// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCell.h"
#include "DanceDefinitions.h"
#include "TilemapLevelManager.h"
#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "DancerHealthComponent.h"
#include "LevelProgressComponent.h"
#include "SongTempoComponent.h"
#include "DancerUIComponent.h"
#include "InventoryComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComponentGetters.generated.h"

namespace ComponentGetters
{
	AChoreoPlayerController* GetController(UWorld* WorldContextObject);
	ADanceCharacter* GetDanceCharacter(UWorld* WorldContextObject);
	UDancerHealthComponent* GetDancerHealthComponent(UWorld* WorldContextObject);
	UInventoryComponent* GetInventoryComponent(UWorld* WorldContextObject);
	ASectionLevelManager* GetSectionLevelManager(UWorld* WorldContextObject);
	ATilemapLevelManager* GetTilemapLevelManager(UWorld* WorldContextObject);
	USongTempoComponent* GetSongTempoComponent(UWorld* WorldContextObject);
	ULevelEventsComponent* GetLevelEventsComponent(UWorld* WorldContextObject);
	ULevelProgressComponent* GetLevelProgressComponent(UWorld* WorldContextObject);
	UDancerUIComponent* GetDancerUIComponent(UWorld* WorldContextObject);
}

UCLASS()
class CHOREOPLATFORMER_API UComponentGettersFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static UDancerHealthComponent* GetDancerHealthComponent(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static ASectionLevelManager* GetSectionLevelManager(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UInventoryComponent* GetInventoryComponent(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static USongTempoComponent* GetSongTempoComponent(AActor* WorldContextActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static AChoreoPlayerController* GetChoreoPlayerController(AActor* WorldContextActor);
};
