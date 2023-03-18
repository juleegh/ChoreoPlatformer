// Fill out your copyright notice in the Description page of Project Settings.


#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DanceCharacter.h"
#include "DanceUtilsFunctionLibrary.h"

USongTempoComponent::USongTempoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USongTempoComponent::BeginPlay()
{
	Super::BeginPlay();
}

float USongTempoComponent::GetAcceptancePercentage()
{
	return UDanceUtilsFunctionLibrary::GetAcceptanceRate();
}

bool USongTempoComponent::IsOnTempo(float target)
{
	if (CurrentPauseTempos > 0)
	{
		return false;
	}
	
	return TempoResult(target) <= GetAcceptancePercentage();
}

float USongTempoComponent::TempoResult(float target)
{
	float Distance = TempoPercentage();
	while (Distance > target)
	{
		Distance -= target;
	}
	return Distance;
}

float USongTempoComponent::TempoPercentage()
{
	float Whole = FMath::FloorToInt(CurrentTime / SongDelay);
	float Residue = (CurrentTime / SongDelay) - Whole;
	return Residue;
}

bool USongTempoComponent::TempoPercentageIsAcceptable(float target)
{
	float Residue = TempoResult(target);
	return Residue < GetAcceptancePercentage() * 0.5f || Residue >= (1 - GetAcceptancePercentage() * 0.5f);
}

void USongTempoComponent::AddPauseTempos(int TemposToAdd)
{
	CurrentPauseTempos += TemposToAdd;
}

void USongTempoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurrentTime += DeltaTime;
	if (!InTempo && TempoPercentageIsAcceptable(1))
	{
		if (CurrentPauseTempos > 0)
		{
			CurrentPauseTempos--;
			if (TempoCountdown.IsBound())
			{
				TempoCountdown.Broadcast(CurrentPauseTempos);
			}
		}
		if (NewTempoStarted.IsBound())
		{
			NewTempoStarted.Broadcast();
		}
	}
	InTempo = TempoPercentageIsAcceptable(1);
}

