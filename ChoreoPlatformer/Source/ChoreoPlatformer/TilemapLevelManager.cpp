// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapLevelManager.h"
#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"
#include "PaperTileSet.h"

ATilemapLevelManager::ATilemapLevelManager()
{

}

void ATilemapLevelManager::BeginPlay()
{
	Super::BeginPlay();
	LoadMap(SelectedMap);
}

void ATilemapLevelManager::LoadMap(UPaperTileMap* TileMap)
{
	auto LayerInfo = TileMap->TileLayers[0];
	auto LayerWidth = LayerInfo->GetLayerWidth();
	auto LayerHeight = LayerInfo->GetLayerHeight();

	for (int column = 0; column < LayerWidth; column++)
	{
		for (int row = 0; row < LayerHeight; row++)
		{
			FString TypeT = "";
			auto TileInfo = LayerInfo->GetCell(column, row);
			auto TileType = TileInfo.PackedTileIndex;
			const FVector DeltaPos = FVector(column * TileInfo.TileSet->GetTileSize().X, row * TileInfo.TileSet->GetTileSize().Y, 0.25) + GetActorLocation();

			if (TileType == 0)
			{
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(BlackTile);
				SpawnedTile->SetActorLocation(DeltaPos);
			}
			else if (TileType == 1)
			{
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(HalfTile);
				SpawnedTile->SetActorLocation(DeltaPos);
			}
			else if (TileType == 2)
			{
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(BlockerTile);
				SpawnedTile->SetActorLocation(DeltaPos);
			}
		}
	}


}


