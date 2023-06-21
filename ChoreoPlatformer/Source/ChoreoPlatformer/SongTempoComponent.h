// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SongTempoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewTempoStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTempoCountdown, int, Tempos);

UCLASS()
class CHOREOPLATFORMER_API USongTempoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USongTempoComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY()
	float SongFrequency;
	UPROPERTY()
	float CalibrationDeficit;
	UPROPERTY()
	bool InTempo;
	UPROPERTY()
	float CurrentTime;
	UPROPERTY()
	int CurrentPauseTempos;
	float GetAcceptancePercentage();
	UPROPERTY()
	bool bIsCountingTempo = false;
public:	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCalibrationDeficit() { return CalibrationDeficit; }
	UPROPERTY(BlueprintAssignable)
	FNewTempoStarted NewTempoStarted;
	UPROPERTY()
	FTempoCountdown TempoCountdown;
	void SetupCalibrationDeficit(float Deficit);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsStopped() { return !bIsCountingTempo; }
	bool IsOnPause() { return CurrentPauseTempos > 0; }
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (IncludeCalibration = "false"))
	bool IsOnTempo(float target, float AcceptancePercentage, bool IncludeCalibration = false);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (IncludeCalibration = "false"))
	float TempoResult(float target, bool IncludeCalibration = false);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float TempoPercentage();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float TempoPercentageWithCalibration();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool TempoPercentageIsAcceptable(float target, float AcceptancePercentage);
	void AddPauseTempos(int);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetFrequency() { return SongFrequency; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetRemainingPauseTempos() { return CurrentPauseTempos; }
	void SetupTempo(float Frequency);
	void StartTempoCounting();
	void StopTempoCounting();
};
