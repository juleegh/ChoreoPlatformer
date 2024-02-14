// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "ChoreoPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCalibrationEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCalibrating);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveBlocked);

UCLASS()
class CHOREOPLATFORMER_API AChoreoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	class USongTempoComponent* SongTempo;
	UPROPERTY()
	class UCalibrationComponent* Calibration;
	UPROPERTY()
	class ULevelProgressComponent* LevelProgress;
	UPROPERTY()
	class UDancerHealthComponent* DancerHealth;
	UPROPERTY()
	class UDancerUIComponent* DancerUI;
	UPROPERTY()
	class UInventoryComponent* Inventory;
	UPROPERTY()
	class ULevelEventsComponent* LevelEvents;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	int IntroductionTempos = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bShouldTakeDamage = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bBypassOutOfTempo = true;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bBypassCalibration = false;
	UPROPERTY()
	bool bIsDead = false;
	UPROPERTY()
	FTimerHandle DelayTimerHandle;
	void CheckMovement(FVector Direction);
	UFUNCTION()
	void OnPlayerDied();
	UFUNCTION()
	void RespawnPlayer();
	void TriggerResultFeedback(float Result);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool InGame();

public:
	AChoreoPlayerController();
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonActivatableWidget> GameUIClass;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPaused();
	UPROPERTY(BlueprintAssignable)
	FMoveBlocked MoveBlocked;
	UPROPERTY(BlueprintAssignable)
	FCalibrating Calibrating;
	UPROPERTY(BlueprintAssignable)
	FCalibrationEnded CalibrationEnded;
	void Move(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void TogglePause();
	UFUNCTION(BlueprintCallable)
	void GoToLevel(const FGameplayTag Level);
	UFUNCTION(BlueprintCallable)
	void GoBackToMainMenu();
	bool ShouldTakeDamage() { return bShouldTakeDamage; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanMove();
	UFUNCTION(BlueprintCallable)
	void TriggerCalibration();
	void PauseGame(const FInputActionValue& Value);
	void FinishCalibration();
};
