// Fill out your copyright notice in the Description page of Project Settings.

#include "TilemapLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GridCell.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"
#include "PaperTileSet.h"

ATilemapLevelManager::ATilemapLevelManager()
{
	
}

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
				SpawnedTile->Initialize((ETempoTile)TileType);
				SpawnedTile->SetOwner(this);
			}
		}

		TileMapActor->SetActorLocation(TileMapActor->GetActorLocation() + FVector::DownVector * 50);
	}
}


