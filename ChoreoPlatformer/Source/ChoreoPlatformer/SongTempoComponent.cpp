// Fill out your copyright notice in the Description page of Project Settings.


#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DanceCharacter.h"

USongTempoComponent::USongTempoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USongTempoComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool USongTempoComponent::IsOnTempo(float target)
{
	if (CurrentPauseTempos > 0)
	{
		return false;
	}
	
	return TempoPercentageIsAcceptable(target);
}

float USongTempoComponent::TempoPercentage(float target)
{
	float Whole = FMath::FloorToInt(CurrentTime / SongDelay * target);
	float Residue = (CurrentTime / SongDelay) - Whole;
	return 1 - Residue;
}

bool USongTempoComponent::TempoPercentageIsAcceptable(float target)
{
	float Residue = TempoPercentage(target);
	return Residue < AcceptancePercentage * 0.5f || Residue >= (target - AcceptancePercentage * 0.5f);
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

