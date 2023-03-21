// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLevelActor.h"
#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"

void AChoreoLevelActor::BeginPlay()
{
	Super::BeginPlay();
	if (auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>())
	{
		SongTempo->AddPauseTempos(IntroTempos);
		SongTempo->SetupTempo(SongFrequency);
		UGameplayStatics::PlaySound2D(GetWorld(), Song);
		SongTempo->StartTempoCounting();
	}
}
