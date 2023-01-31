// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoController.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoController>(TEXT("Song Tempo"));
	SongTempo->SetupAttachment(GetRootComponent());
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
}
