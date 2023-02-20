// Fill out your copyright notice in the Description page of Project Settings.

#include "TilemapLevelManager.h"
#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
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

	if (FoundActors.Num() == 0)
	{
		return;
	}
	auto TileMapActor = FoundActors[0];
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
			auto TileType = TileInfo.PackedTileIndex;
			const FVector DeltaPos = GetActorLocation() + GetActorRightVector() * row * TileInfo.TileSet->GetTileSize().X + GetActorForwardVector() * column * TileInfo.TileSet->GetTileSize().Y;

			if (TileType == 0)
			{
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(BlackTile, DeltaPos, GetActorRotation());
				SpawnedTile->SetOwner(this);
			}
			else if (TileType == 1)
			{
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(HalfTile, DeltaPos, GetActorRotation());
				SpawnedTile->SetOwner(this);
			}
			else if (TileType == 2)
			{
				auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(BlockerTile, DeltaPos, GetActorRotation());
				SpawnedTile->SetOwner(this);
			}
		}
	}

	TileMapActor->SetActorLocation(TileMapActor->GetActorLocation() + FVector::DownVector * 50);
}


