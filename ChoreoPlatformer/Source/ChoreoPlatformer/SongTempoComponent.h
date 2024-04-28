// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SongTempoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewTempoStarted);

UCLASS()
class CHOREOPLATFORMER_API USongTempoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USongTempoComponent();

protected:
	UPROPERTY()
	float SongFrequency;
	UPROPERTY()
	float CalibrationDeficit;
	UPROPERTY()
	bool InTempo;
	UPROPERTY()
	float CurrentTime;
	float GetAcceptancePercentage();
	UPROPERTY()
	class ADanceAudioManager* DanceAudioManager;
	UPROPERTY()
	bool bIsCountingTempo = false;
public:	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCalibrationDeficit() { return CalibrationDeficit; }
	UPROPERTY(BlueprintAssignable)
	FNewTempoStarted NewTempoStarted;
	void SetupCalibrationDeficit(float Deficit);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsStopped() { return !bIsCountingTempo; }
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetFrequency() { return SongFrequency; }
	void SetupTempo(float Frequency);
	void StartTempoCounting();
	void StopTempoCounting();
};
