// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "CalibrationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCalibrationEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCalibrating);

UCLASS()
class CHOREOPLATFORMER_API UCalibrationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCalibrationComponent();

protected:
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	bool bIsCalibrated = true;
	UPROPERTY()
	TArray<float> UsedTempos;
	UPROPERTY()
	float PostTempoMargin;
	UPROPERTY()
	int Tries = 0;
	float GetCalibrationDelta();
	void ProcessCurrentInput(float Percentage);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCalibrated() const;
	UFUNCTION(BlueprintCallable)
	void ReceiveInput();
	UFUNCTION(BlueprintCallable)
	void TriggerCalibration();
	void StartCalibration();
	void UseInputPassive();

	UPROPERTY(BlueprintAssignable)
	FCalibrating Calibrating;
	UPROPERTY(BlueprintAssignable)
	FCalibrationEnded CalibrationEnded;
};
