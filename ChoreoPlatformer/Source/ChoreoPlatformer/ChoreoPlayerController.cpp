// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoController.h"
#include "DanceCharacter.h"
#include "GridCell.h"
#include "TilemapLevelManager.h"
#include "TileDetectorComponent.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoController>(TEXT("Song Tempo"));
	TileDetector = CreateDefaultSubobject<UTileDetectorComponent>(TEXT("Tile Detector"));
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DanceCharacter = Cast<ADanceCharacter>(GetPawn());
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
	if (TileDetector->CheckTile(DanceCharacter->GetActorLocation() + Direction * 100) == ETempoTile::Blocker)
	{
		return;
	}

	if (TileDetector->CheckTile(DanceCharacter->GetActorLocation() + Direction * 100) == ETempoTile::None)
	{
		return;
	}

	if (SongTempo->IsOnTempo())
	{
		DanceCharacter->MoveInDirection(Direction);
	}
}

