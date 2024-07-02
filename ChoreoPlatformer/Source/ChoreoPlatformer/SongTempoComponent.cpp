// Fill out your copyright notice in the Description page of Project Settings.


#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ComponentGetters.h"
#include "DanceAudioManager.h"
#include "DanceUtilsFunctionLibrary.h"

USongTempoComponent::USongTempoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USongTempoComponent::SetupCalibrationDeficit(float Deficit)
{
	CalibrationDeficit = Deficit * SongFrequency;
	UE_LOG(LogTemp, Warning, TEXT("CalibrationDeficit: %f"), Deficit);

}

float USongTempoComponent::GetAcceptancePercentage()
{
	return UDanceUtilsFunctionLibrary::GetAcceptanceRate();
}

bool USongTempoComponent::IsOnTempo(float target, float AcceptancePercentage, bool IncludeCalibration)
{
	return TempoResult(target, IncludeCalibration) <= AcceptancePercentage;
}

float USongTempoComponent::TempoResult(float target, bool IncludeCalibration)
{
	float Distance = IncludeCalibration ? TempoPercentageWithCalibration() : TempoPercentage();
	float MinDistance = Distance;
	for (float step = 0; step <= 1; step += target)
	{
		if (FMath::Abs(step - Distance) < MinDistance)
		{
			MinDistance = FMath::Abs(step - Distance);
		}
	}
	return MinDistance;
}

float USongTempoComponent::TempoPercentageWithCalibration()
{
	float Whole = FMath::FloorToInt((CurrentTime + CalibrationDeficit) / SongFrequency);
	float Residue = ((CurrentTime + CalibrationDeficit) / SongFrequency) - Whole;
	return Residue;
}

float USongTempoComponent::TempoPercentage()
{
	float Whole = FMath::FloorToInt((CurrentTime) / SongFrequency);
	float Residue = ((CurrentTime) / SongFrequency) - Whole;
	return Residue;
}

bool USongTempoComponent::TempoPercentageIsAcceptable(float target, float AcceptancePercentage)
{
	float Residue = TempoPercentage();
	return Residue < AcceptancePercentage * 0.5f || Residue >= (1 - AcceptancePercentage * 0.5f);
}

void USongTempoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsCountingTempo)
	{
		return;
	}
	CurrentTime += DeltaTime;
	if (!InTempo && TempoPercentageIsAcceptable(1, GetAcceptancePercentage()))
	{
		if (NewTempoStarted.IsBound())
		{
			NewTempoStarted.Broadcast();
		}
		if (!DanceAudioManager)
		{
			DanceAudioManager = ComponentGetters::GetDanceAudioManager(GetWorld());
		}
		DanceAudioManager->MarkTempo();
	}
	InTempo = TempoPercentageIsAcceptable(1, GetAcceptancePercentage());
}

void USongTempoComponent::SetupTempo(float Frequency)
{
	SongFrequency = Frequency;
	CurrentTime = 0;
}

void USongTempoComponent::StartTempoCounting()
{
	CurrentTime = 0;
	bIsCountingTempo = true;
}

void USongTempoComponent::StopTempoCounting()
{
	bIsCountingTempo = false;
}

