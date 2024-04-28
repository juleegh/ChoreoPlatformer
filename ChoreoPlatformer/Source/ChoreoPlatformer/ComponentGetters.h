// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComponentGetters.generated.h"

class AChoreoPlayerController;
class ADanceCharacter;
class UDancerHealthComponent;
class UInventoryComponent;
class ASectionLevelManager;
class ATilemapLevelManager;
class AEndlessLevelManager;
class ADanceAudioManager;
class USongTempoComponent;
class UCalibrationComponent;
class ULevelEventsComponent;
class ULevelProgressComponent;
class UDancerUIComponent;
class ASectionStart;
struct FGameplayTag;

namespace ComponentGetters
{
	AChoreoPlayerController* GetController(UWorld* WorldContextObject);
	ADanceCharacter* GetDanceCharacter(UWorld* WorldContextObject);
	UDancerHealthComponent* GetDancerHealthComponent(UWorld* WorldContextObject);
	UInventoryComponent* GetInventoryComponent(UWorld* WorldContextObject);
	ASectionLevelManager* GetSectionLevelManager(UWorld* WorldContextObject);
	ATilemapLevelManager* GetTilemapLevelManager(UWorld* WorldContextObject);
	AEndlessLevelManager* GetEndlessLevelManager(UWorld* WorldContextObject);
	ADanceAudioManager* GetDanceAudioManager(UWorld* WorldContextObject);
	USongTempoComponent* GetSongTempoComponent(UWorld* WorldContextObject);
	ULevelEventsComponent* GetLevelEventsComponent(UWorld* WorldContextObject);
	ULevelProgressComponent* GetLevelProgressComponent(UWorld* WorldContextObject);
	UDancerUIComponent* GetDancerUIComponent(UWorld* WorldContextObject);
	UCalibrationComponent* GetCalibrationComponent(UWorld* WorldContextObject);
	ASectionStart* GetSectionStart(UWorld* WorldContextObject, const FGameplayTag& SectionIdentifier);
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
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextActor"))
	static ULevelEventsComponent* GetLevelEventsComponent(AActor* WorldContextActor);
};
