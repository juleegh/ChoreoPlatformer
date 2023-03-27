// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "DanceCharacter.h"
#include "GridCell.h"
#include "TilemapLevelManager.h"
#include "TileDetectorComponent.h"
#include "LevelProgressComponent.h"
#include "DancerHealthComponent.h"
#include "DancerUIComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "TimelineCreatorComponent.h"
#include "ContextualElement.h"
#include "Kismet/GameplayStatics.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoComponent>(TEXT("Song Tempo"));
	TileDetector = CreateDefaultSubobject<UTileDetectorComponent>(TEXT("Tile Detector"));
	LevelProgress = CreateDefaultSubobject<ULevelProgressComponent>(TEXT("Level Progress"));
	DancerHealth = CreateDefaultSubobject<UDancerHealthComponent>(TEXT("Dancer Health"));
	DancerUI = CreateDefaultSubobject<UDancerUIComponent>(TEXT("Dancer UI"));
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DanceCharacter = Cast<ADanceCharacter>(GetPawn());
	DancerHealth->PlayerDied.AddDynamic(this, &AChoreoPlayerController::OnPlayerDied);
	DancerHealth->HealthChanged.AddDynamic(DancerUI, &UDancerUIComponent::UpdateHealth);
	SongTempo->TempoCountdown.AddDynamic(DancerUI,&UDancerUIComponent::UpdateCountdown);
	DanceCharacter->GetMovementTimeline()->TimelineEnded.AddDynamic(this, &AChoreoPlayerController::OnFinishedMovement);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectionLevelManager::StaticClass(), FoundActors);

	for (auto SectionActor : FoundActors)
	{
		SectionManager = Cast<ASectionLevelManager>(SectionActor);
		break;
	}
}

void AChoreoPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChoreoPlayerController::PressedUp()
{
	CheckMovement(-FVector::RightVector);
}

void AChoreoPlayerController::PressedDown()
{
	CheckMovement(FVector::RightVector);
}

void AChoreoPlayerController::PressedLeft()
{
	CheckMovement(-FVector::ForwardVector);
}

void AChoreoPlayerController::PressedRight()
{
	CheckMovement(FVector::ForwardVector);
}

void AChoreoPlayerController::CheckMovement(FVector Direction)
{
	FDetectedInfo CurrentTile = TileDetector->CheckPosition(DanceCharacter->GetActorLocation());
	float Result = SongTempo->TempoResult(CurrentTile.TargetTempo);

	FVector TargetPosition = UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), Direction);
	FDetectedInfo NextTile = TileDetector->CheckPosition(TargetPosition);
	if (NextTile.bHitElement)
	{
		NextTile.HitElement->TriggerInteraction();
		DancerUI->PromptTempoResult(Result);
		return;
	}
	else if (NextTile.TileType == ETempoTile::Blocker)
	{
		return;
	}
	else if(SectionManager)
	{
		SectionManager->SectionChanged(NextTile.Section);
	}

	DancerUI->PromptTempoResult(Result);
	if (SongTempo->IsOnTempo(CurrentTile.TargetTempo))
	{
		DanceCharacter->MoveInDirection(Direction);
	}
}

void AChoreoPlayerController::OnFinishedMovement()
{
	FDetectedInfo CurrentTile = TileDetector->CheckPosition(DanceCharacter->GetActorLocation());

	if (CurrentTile.bForcesDirection)
	{
		DanceCharacter->MoveInDirection(CurrentTile.ForcedDirection);
	}
}

void AChoreoPlayerController::OnPlayerDied()
{
	DanceCharacter->StopMovement();
	LevelProgress->LoadCheckpoint();
}


