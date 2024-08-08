// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCell.h"
#include "DanceDefinitions.h"
#include "CalibrationComponent.h"
#include "GameCameraComponent.h"
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static class ADanceCharacter* GetDanceCharacter(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UCalibrationComponent* GetCalibrationComponent(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UGameCameraComponent* GetGameCameraComponent(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static class UInventoryComponent* GetInventoryComponent(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool EnemyCanMove(AActor* Enemy);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool ActorIsOnTempo(AActor* Player);
};
