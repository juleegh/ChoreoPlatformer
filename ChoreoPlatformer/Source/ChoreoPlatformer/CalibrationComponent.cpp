// Fill out your copyright notice in the Description page of Project Settings.


#include "CalibrationComponent.h"
#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DanceCharacter.h"
#include "DanceUtilsFunctionLibrary.h"

UCalibrationComponent::UCalibrationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UCalibrationComponent::IsCalibrated()
{
	return bIsCalibrated;
}

float UCalibrationComponent::GetCalibrationDelta()
{
	int Pre = PreTempos == 0 ? 1 : PreTempos;
	return PreTempoMargin / PreTempos;
}


void UCalibrationComponent::ReceiveInput()
{
	if (!SongTempo)
	{
		SongTempo = GetOwner()->FindComponentByClass<USongTempoComponent>();
	}
	PreTempos++;
	PreTempoMargin += SongTempo->TempoPercentage();
	SongTempo->SetupCalibrationDeficit(GetCalibrationDelta());

	if (PreTempos == 8)
	{
		bIsCalibrated = true;
	}
}