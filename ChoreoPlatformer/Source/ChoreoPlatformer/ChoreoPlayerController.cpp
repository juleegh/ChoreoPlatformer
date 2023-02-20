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
}

void AChoreoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChoreoPlayerController::PressedUp()
{
	CheckMovement(DanceCharacter->GetActorForwardVector());
}

void AChoreoPlayerController::PressedDown()
{
	CheckMovement(-DanceCharacter->GetActorForwardVector());
}

void AChoreoPlayerController::PressedLeft()
{
	CheckMovement(-DanceCharacter->GetActorRightVector());
}

void AChoreoPlayerController::PressedRight()
{
	CheckMovement(DanceCharacter->GetActorRightVector());
}

void AChoreoPlayerController::CheckMovement(FVector Direction)
{
	auto NextBlock = TileDetector->CheckTile(UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), Direction));
	if (NextBlock == ETempoTile::Blocker)
	{
		return;
	}

	if (SongTempo->IsOnTempo(UDanceUtilsFunctionLibrary::GetTargetTempo(TileDetector->CheckTile(DanceCharacter->GetActorLocation()))))
	{
		DanceCharacter->MoveInDirection(Direction);
	}
}

void AChoreoPlayerController::OnPlayerDied()
{
	DanceCharacter->StopMovement();
	LevelProgress->LoadCheckpoint();
}


