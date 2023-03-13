// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "DanceCharacter.h"
#include "GridCell.h"
#include "TilemapLevelManager.h"
#include "TileDetectorComponent.h"
#include "LevelProgressComponent.h"
#include "DancerHealthComponent.h"
#include "DancerUIComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "TimelineCreatorComponent.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoComponent>(TEXT("Song Tempo"));
	TileDetector = CreateDefaultSubobject<UTileDetectorComponent>(TEXT("Tile Detector"));
	LevelProgress = CreateDefaultSubobject<ULevelProgressComponent>(TEXT("Level Progress"));
	DancerHealth = CreateDefaultSubobject<UDancerHealthComponent>(TEXT("Dancer Health"));
	DancerUI = CreateDefaultSubobject<UDancerUIComponent>(TEXT("Dancer UI"));
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DanceCharacter = Cast<ADanceCharacter>(GetPawn());
	DancerHealth->PlayerDied.AddDynamic(this, &AChoreoPlayerController::OnPlayerDied);
	DancerHealth->HealthChanged.AddDynamic(DancerUI, &UDancerUIComponent::UpdateHealth);
	DanceCharacter->GetMovementTimeline()->TimelineEnded.AddDynamic(this, &AChoreoPlayerController::OnFinishedMovement);
}

void AChoreoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChoreoPlayerController::PressedUp()
{
	CheckMovement(-FVector::RightVector);
}

void AChoreoPlayerController::PressedDown()
{
	CheckMovement(FVector::RightVector);
}

void AChoreoPlayerController::PressedLeft()
{
	CheckMovement(-FVector::ForwardVector);
}

void AChoreoPlayerController::PressedRight()
{
	CheckMovement(FVector::ForwardVector);
}

void AChoreoPlayerController::CheckMovement(FVector Direction)
{
	auto NextBlock = TileDetector->CheckTile(UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), Direction))->GetTileType();
	if (NextBlock == ETempoTile::Blocker)
	{
		return;
	}

	if (SongTempo->IsOnTempo(UDanceUtilsFunctionLibrary::GetTargetTempo(TileDetector->CheckTile(DanceCharacter->GetActorLocation())->GetTileType())))
	{
		DanceCharacter->MoveInDirection(Direction);
	}
}

void AChoreoPlayerController::OnFinishedMovement()
{
	auto NewBlock = TileDetector->CheckTile(DanceCharacter->GetActorLocation());

	if (NewBlock->ForcesPlayerPosition())
	{
		DanceCharacter->MoveInDirection(NewBlock->ForcedDirection());
	}
}

void AChoreoPlayerController::OnPlayerDied()
{
	DanceCharacter->StopMovement();
	LevelProgress->LoadCheckpoint();
}


