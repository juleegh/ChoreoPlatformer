// Fill out your copyright notice in the Description page of Project Settings.

#include "TilemapLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GridCell.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"
#include "PaperTileSet.h"
#include "GameplayTagContainer.h"
#include "SongTempoComponent.h"
#include "ChoreoPlayerController.h"
#include "LevelEventsComponent.h"
#include "DanceCharacter.h"
#include "DanceUtilsFunctionLibrary.h"

void ATilemapLevelManager::BeginPlay()
{
	Super::BeginPlay();
	LoadMap();
}

void ATilemapLevelManager::LoadMap()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaperTileMapActor::StaticClass(), FoundActors);

	for (auto TileMapActor : FoundActors)
	{
		auto TileMap = Cast<APaperTileMapActor>(TileMapActor)->GetRenderComponent()->TileMap;
		auto LayerInfo = TileMap->TileLayers[0];
		auto LayerName = FName(LayerInfo->LayerName.ToString());
		FGameplayTag SectionIdentifier = FGameplayTag::RequestGameplayTag(LayerName, false);
		if (!SectionIdentifier.IsValid())
		{
			continue;
		}
		auto LayerWidth = LayerInfo->GetLayerWidth();
		auto LayerHeight = LayerInfo->GetLayerHeight();

		for (int column = 0; column < LayerWidth; column++)
		{
			for (int row = 0; row < LayerHeight; row++)
			{
				FString TypeT = "";
				auto TileInfo = LayerInfo->GetCell(column, row);

				if (TileInfo.TileSet == nullptr)
					continue;

				auto TileType = TileInfo.PackedTileIndex;
				const FVector DeltaPos = TileMapActor->GetActorLocation() + GetActorRightVector() * row * TileInfo.TileSet->GetTileSize().X + GetActorForwardVector() * column * TileInfo.TileSet->GetTileSize().Y;
				
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(TileBP, DeltaPos, GetActorRotation());
				SpawnedTile->Initialize((ETempoTile)TileType, SectionIdentifier);
				SpawnedTile->SetOwner(this);
			}
		}

		TileMapActor->SetActorLocation(TileMapActor->GetActorLocation() + FVector::DownVector * 50);
	}

	TotalChallenges.Add(EChallengeType::HalfCoin, 0);
	TotalChallenges.Add(EChallengeType::CoinTrail, 0);
	CollectedChallenges.Add(EChallengeType::HalfCoin, 0);
	CollectedChallenges.Add(EChallengeType::CoinTrail, 0);
	
	TArray<AActor*> FoundChallenges;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileChallenge::StaticClass(), FoundChallenges);
	
	for (auto Challenge : FoundChallenges)
	{
		if (auto TrailChallenge = Cast<ACoinTrail>(Challenge))
		{
			TotalChallenges[EChallengeType::CoinTrail]++;
		}
		else if (auto HalfChallenge = Cast<AHalfCoin>(Challenge))
		{
			TotalChallenges[EChallengeType::HalfCoin]++;
		}
	}

}

void ATilemapLevelManager::CollectChallenge(EChallengeType ChallengeType)
{
	CollectedChallenges[ChallengeType]++;
}

int ATilemapLevelManager::GetTotalByChallengeType(EChallengeType ChallengeType)
{
	return TotalChallenges[ChallengeType];
}

int ATilemapLevelManager::GetCollectedByChallengeType(EChallengeType ChallengeType)
{
	return CollectedChallenges[ChallengeType];
}

void ASectionLevelManager::BeginPlay()
{
	Super::BeginPlay();
	if (auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>())
	{
		SongTempo->AddPauseTempos(IntroTempos);
		SongTempo->SetupTempo(60 / SongBPM);
		SongTempo->StartTempoCounting();

		auto DanceCharacter = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		DanceCharacter->SetupToLevel();
		FTileInfo StartTile = UDanceUtilsFunctionLibrary::CheckPosition(DanceCharacter, DanceCharacter->GetActorLocation());
		SectionChanged(StartTile.Section);
	}
}

void ASectionLevelManager::SectionChanged(FGameplayTag NewSection)
{
	if (!NewSection.IsValid())
	{
		return;
	}

	if (!CurrentSection.IsValid() || CurrentSection != NewSection)
	{
		CurrentSection = NewSection;
		auto LevelEvents = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetEventsComponent();
		LevelEvents->ActivateTrigger(NewSection);
		if (Sections.Contains(CurrentSection))
		{
			CurrentSectionStart = Sections[CurrentSection].BeatStart;
			CurrentSectionDuration = Sections[CurrentSection].BeatDuration;
			PlayCurrentSection();
		}
	}
}


