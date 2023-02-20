// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
#include "SongTempoComponent.h"
#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"

// Sets default values
AGridCell::AGridCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

