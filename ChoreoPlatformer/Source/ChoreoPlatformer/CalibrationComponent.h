// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Components/ActorComponent.h"
#include "CalibrationComponent.generated.h"

UCLASS()
class CHOREOPLATFORMER_API UCalibrationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCalibrationComponent();

protected:
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	bool bIsCalibrated = false;
	UPROPERTY()
	float PreTempos = 0;
	UPROPERTY()
	float PreTempoMargin;
	float GetCalibrationDelta();

public:
	bool IsCalibrated();
	void ReceiveInput();
};

UCLASS()
class CHOREOPLATFORMER_API USaveData : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	double CalibrationDelta;
};
