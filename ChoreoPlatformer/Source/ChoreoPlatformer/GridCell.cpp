// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
#include "SongTempoComponent.h"
#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"

// Sets default values
AGridCell::AGridCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGridCell::Initialize(ETempoTile Properties)
{
	TileType = Properties;
	PaintTile();
}


// Called when the game starts or when spawned
void AGridCell::BeginPlay()
{
	Super::BeginPlay();
	auto PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	SongTempo = PlayerController->GetSongTempoComponent();
}

// Called every frame
void AGridCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGridCell::ForcesPlayerPosition()
{
	return TileType == ETempoTile::ForceUp ||
		TileType == ETempoTile::ForceDown ||
		TileType == ETempoTile::ForceLeft ||
		TileType == ETempoTile::ForceRight;
}

FVector AGridCell::ForcedDirection()
{
	switch (TileType)
	{
		case ETempoTile::ForceUp:
			return -FVector::RightVector;
		case ETempoTile::ForceDown:
			return FVector::RightVector;
		case ETempoTile::ForceRight:
			return FVector::ForwardVector;
		case ETempoTile::ForceLeft:
			return -FVector::ForwardVector;
		default:
			return FVector::ZeroVector;
	}
}


