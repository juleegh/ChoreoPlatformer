// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
#include "SongTempoController.h"
#include "ChoreoPlayerController.h"

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
	
}
bool AGridCell::IsOnTempo()
{
	auto Controller = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	auto TempoController = Controller->GetSongTempoController();
	return TempoController->IsOnTempo();
}


// Called every frame
void AGridCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

