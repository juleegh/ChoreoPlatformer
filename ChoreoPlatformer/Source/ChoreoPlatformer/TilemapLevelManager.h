// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TilemapLevelManager.generated.h"

class UPaperTileMap;
class AGridCell;

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ATilemapLevelManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ATilemapLevelManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	void LoadMap();
	UPROPERTY(EditDefaultsOnly, Category = "Tile Types")
	TSubclassOf<AGridCell> BlockerTile;
	UPROPERTY(EditDefaultsOnly, Category = "Tile Types")
	TSubclassOf<AGridCell> BlackTile;
	UPROPERTY(EditDefaultsOnly, Category = "Tile Types")
	TSubclassOf<AGridCell> HalfTile;
};
