// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelProgressComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GridElement.h"
#include "DanceCharacter.h"
#include "ComponentGetters.h"
#include "GameFramework/PlayerStart.h"

ULevelProgressComponent::ULevelProgressComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULevelProgressComponent::ToggleCheckpoint(ACheckpoint* NewCheckpoint)
{
	CurrentCheckpoint = NewCheckpoint;
}

void ULevelProgressComponent::LoadCheckpoint()
{
	FVector RespawnPos;

	if (CurrentCheckpoint)
	{
		RespawnPos = CurrentCheckpoint->GetActorLocation();
	}
	else
	{
		const UWorld* World = GetWorld();
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
		for (auto Start : PlayerStarts)
		{
			RespawnPos = Start->GetActorLocation();
			break;
		}
	}
	if (auto DanceCharacter = ComponentGetters::GetDanceCharacter(GetWorld()))
	{
		DanceCharacter->Respawn(RespawnPos);
	}
}
