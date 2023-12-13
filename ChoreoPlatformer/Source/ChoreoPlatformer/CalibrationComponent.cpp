// Fill out your copyright notice in the Description page of Project Settings.


#include "CalibrationComponent.h"
#include "ChoreoPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"

UCalibrationComponent::UCalibrationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCalibrationComponent::StartCalibration()
{
	bIsCalibrated = false;
	PostTempos = 0;
	PostTempoMargin = 0;
	Tries = 0;
}


bool UCalibrationComponent::IsCalibrated()
{
	return bIsCalibrated;
}

float UCalibrationComponent::GetCalibrationDelta()
{
	if (PostTempos == 0)
		return 0;
	return (PostTempoMargin / PostTempos);
}


void UCalibrationComponent::ReceiveInput()
{
	if (bIsCalibrated)
	{
		return;
	}

	if (!SongTempo)
	{
		SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	}
	float Percentage = SongTempo->TempoPercentage();
	PostTempos++;
	if (Percentage > 0.5f)
	{
		PostTempoMargin += 1 - Percentage;
	}
	else
	{
		PostTempoMargin -= Percentage;
	}

	Tries++;
	SongTempo->SetupCalibrationDeficit(GetCalibrationDelta());

	UE_LOG(LogTemp, Warning, TEXT("Deviation: %f"), Percentage);
	if (SongTempo->IsOnTempo(1, UDanceUtilsFunctionLibrary::GetAcceptanceRate(), true) && Tries >= 8)
	{
		bIsCalibrated = true;
		Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->FinishCalibration();
	}
}
