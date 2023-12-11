// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelProgressComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GridElement.h"
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
	APawn* character = Cast<AController>(GetOwner())->GetPawn();

	if (CurrentCheckpoint)
	{
		character->SetActorLocation(CurrentCheckpoint->GetActorLocation());
	}
	else
	{
		const UWorld* World = GetWorld();
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
		for (auto Start : PlayerStarts)
		{
			character->SetActorLocation(Start->GetActorLocation());
			break;
		}
	}
}
