// Fill out your copyright notice in the Description page of Project Settings.

#include "TilemapLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GridCell.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "ContextualElement.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"
#include "PaperTileSet.h"
#include "GameplayTagContainer.h"
#include "SongTempoComponent.h"
#include "ChoreoPlayerController.h"
#include "LevelEventsComponent.h"
#include "DanceCharacter.h"

void ATilemapLevelManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> LevelManager;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectionLevelManager::StaticClass(), LevelManager);

	for (auto Manager : LevelManager)
	{
		if (auto Level = Cast<ASectionLevelManager>(Manager))
		{
			Level->Initialize();
			break;
		}
	}
	LoadMap();
}

void ATilemapLevelManager::LoadMap()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaperTileMapActor::StaticClass(), FoundActors);

	for (auto TileMapActor : FoundActors)
	{
		auto TileMap = Cast<APaperTileMapActor>(TileMapActor)->GetRenderComponent()->TileMap;
		auto FirstLayer = TileMap->TileLayers.Last();
		auto LayerName = FName(FirstLayer->LayerName.ToString());
		FGameplayTag SectionIdentifier = FGameplayTag::RequestGameplayTag(LayerName, false);
		if (!SectionIdentifier.IsValid())
		{
			continue;
		}
		auto LayerWidth = FirstLayer->GetLayerWidth();
		auto LayerHeight = FirstLayer->GetLayerHeight();

		float LayerPos = 0;
		for (auto LayerInfo : TileMap->TileLayers)
		{
			for (int column = 0; column < LayerWidth; column++)
			{
				for (int row = 0; row < LayerHeight; row++)
				{
					FString TypeT = "";
					auto TileInfo = LayerInfo->GetCell(column, row);

					if (TileInfo.TileSet == nullptr)
						continue;

					auto TileType = TileInfo.PackedTileIndex;
					const FVector DeltaPos = TileMapActor->GetActorLocation() + GetActorRightVector() * row * TileInfo.TileSet->GetTileSize().X + GetActorForwardVector() * column * TileInfo.TileSet->GetTileSize().Y + GetActorUpVector() * LayerPos;

					SpawnTile(DeltaPos, (ETempoTile)TileType, SectionIdentifier);
				}
			}
			LayerPos -= 50.f;
		}

		TileMapActor->SetActorHiddenInGame(true);
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

	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AContextualElement::StaticClass(), FoundItems);

	for (auto FoundItem : FoundItems)
	{
		if (auto Item = Cast<AItem>(FoundItem))
		{
			if (Item->GetItemType().GetGameplayTagParents().HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Health"))))
			{
				TotalFruit++;
			}
		}
	}

}

void ATilemapLevelManager::SpawnTile(FVector Position, ETempoTile TileType, FGameplayTag SectionIdentifier)
{
	auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(TileBP, Position, GetActorRotation());
	SpawnedTile->Initialize(TileType, SectionIdentifier);
	SpawnedTile->SetOwner(this);
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

int ATilemapLevelManager::GetTotalFruit()
{
	return TotalFruit;
}

void ASectionLevelManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASectionLevelManager::Initialize()
{
	SectionChanged(StartSection);

	auto DanceCharacter = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	DanceCharacter->SetupToLevel();

	if (auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>())
	{
		SongTempo->StartTempoCounting();
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
			SectionSong = Sections[CurrentSection].Song;
			auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
			CurrentSongBPM = Sections[CurrentSection].SongBPM;
			SongTempo->SetupTempo(60 / Sections[CurrentSection].SongBPM);
			PlayCurrentSection();
		}
	}
}


