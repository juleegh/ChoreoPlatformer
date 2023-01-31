// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoController.h"
#include "DanceCharacter.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoController>(TEXT("Song Tempo"));
	SongTempo->SetupAttachment(GetRootComponent());
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DanceCharacter = Cast<ADanceCharacter>(GetPawn());
}

void AChoreoPlayerController::PressedUp()
{
	if (SongTempo->IsOnTempo())
	{
		DanceCharacter->MoveInDirection(FVector::XAxisVector);
	}
}

void AChoreoPlayerController::PressedDown()
{
	if (SongTempo->IsOnTempo())
	{
		DanceCharacter->MoveInDirection(-FVector::XAxisVector);
	}
}

void AChoreoPlayerController::PressedLeft()
{
	if (SongTempo->IsOnTempo())
	{
		DanceCharacter->MoveInDirection(-FVector::YAxisVector);
	}
}

void AChoreoPlayerController::PressedRight()
{
	if (SongTempo->IsOnTempo())
	{
		DanceCharacter->MoveInDirection(FVector::YAxisVector);
	}
}
