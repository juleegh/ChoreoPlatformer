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
	float Residue = TempoPercentage(target);
	return Residue < AcceptancePercentage * 0.5f || Residue >= (target - AcceptancePercentage * 0.5f);
}

float USongTempoComponent::TempoPercentage(float target)
{
	float Whole = FMath::FloorToInt(CurrentTime / SongDelay * target);
	float Residue = (CurrentTime / SongDelay) - Whole;
	return 1 - Residue;
}

void USongTempoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurrentTime += DeltaTime;
}

