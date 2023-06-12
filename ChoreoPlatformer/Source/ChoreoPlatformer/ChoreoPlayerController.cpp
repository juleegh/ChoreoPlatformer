// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "DanceCharacter.h"
#include "GridCell.h"
#include "TilemapLevelManager.h"
#include "LevelProgressComponent.h"
#include "DancerHealthComponent.h"
#include "DancerUIComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "TimelineCreatorComponent.h"
#include "ContextualElement.h"
#include "LevelEventsComponent.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoComponent>(TEXT("Song Tempo"));
	LevelProgress = CreateDefaultSubobject<ULevelProgressComponent>(TEXT("Level Progress"));
	DancerHealth = CreateDefaultSubobject<UDancerHealthComponent>(TEXT("Dancer Health"));
	DancerUI = CreateDefaultSubobject<UDancerUIComponent>(TEXT("Dancer UI"));
	LevelEvents = CreateDefaultSubobject<ULevelEventsComponent>(TEXT("Level Events"));
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
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
	if (SongTempo->IsOnPause() || SongTempo->IsStopped())
	{
		return;
	}

	CheckForTileManager();

	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition(DanceCharacter, DanceCharacter->GetActorLocation());
	float Result = SongTempo->TempoResult(CurrentTile.TargetTempo);

	FVector TargetPosition = UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), Direction);
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition(DanceCharacter, TargetPosition);
	if (NextTile.TileType == ETempoTile::Blocker)
	{
		return;
	}

	TriggerResultFeedback(Result);
	
	if (NextTile.bHitElement)
	{
		NextTile.HitElement->TriggerInteraction();
		return;
	}

	if (SongTempo->IsOnTempo(CurrentTile.TargetTempo) || !bShouldPunishTempo)
	{
		DanceCharacter->MoveInDirection(Direction);
		SectionManager->SectionChanged(NextTile.Section);
	}
}

void AChoreoPlayerController::OnFinishedMovement()
{
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition(DanceCharacter, DanceCharacter->GetActorLocation());

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

void AChoreoPlayerController::CheckForTileManager()
{
	if (!SectionManager)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectionLevelManager::StaticClass(), FoundActors);

		for (auto SectionActor : FoundActors)
		{
			SectionManager = Cast<ASectionLevelManager>(SectionActor);
			break;
		}
	}
}

void AChoreoPlayerController::TriggerResultFeedback(float Result)
{
	DancerHealth->CountStep(UDanceUtilsFunctionLibrary::GetTempoResult(Result));
	DancerUI->PromptTempoResult(Result);
	SectionManager->PlayTempoResult(UDanceUtilsFunctionLibrary::GetTempoResult(Result));
}




