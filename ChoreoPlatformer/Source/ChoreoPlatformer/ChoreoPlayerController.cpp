// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "CalibrationComponent.h"
#include "DanceCharacter.h"
#include "GridCell.h"
#include "TilemapLevelManager.h"
#include "LevelProgressComponent.h"
#include "DancerHealthComponent.h"
#include "DancerUIComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "TimelineCreatorComponent.h"
#include "ContextualElement.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

AChoreoPlayerController::AChoreoPlayerController()
{
	SongTempo = CreateDefaultSubobject<USongTempoComponent>(TEXT("Song Tempo"));
	Calibration = CreateDefaultSubobject<UCalibrationComponent>(TEXT("Controller Calibration"));
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
	SongTempo->TempoCountdown.AddDynamic(DancerUI,&UDancerUIComponent::UpdateCountdown);
	DanceCharacter->GetMovementTimeline()->TimelineEnded.AddDynamic(this, &AChoreoPlayerController::OnFinishedMovement);
	if (bBypassCalibration)
	{
		CalibrationEnded.Broadcast();
		LevelEvents->ActivateTrigger(FGameplayTag::RequestGameplayTag(FName("tutorial.intro")));
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
	if (!Calibration->IsCalibrated() && !bBypassCalibration)
	{
		Calibration->ReceiveInput();
		Calibrating.Broadcast();
		return;
	}
	if (SongTempo->IsOnPause() || SongTempo->IsStopped())
	{
		return;
	}

	CheckForTileManager();

	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition(DanceCharacter, DanceCharacter->GetActorLocation());
	float Result = SongTempo->TempoResult(CurrentTile.TargetTempo, true);

	FVector TargetPosition = UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), Direction);
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition(DanceCharacter, TargetPosition);
	if (NextTile.TileType == ETempoTile::Blocker || (CurrentTile.bForcesDirection && CurrentTile.ForcedDirection != Direction))
	{
		return;
	}

	TriggerResultFeedback(Result);
	
	if (NextTile.bHitElement)
	{
		NextTile.HitElement->TriggerInteraction();
		return;
	}

	if (SongTempo->IsOnTempo(CurrentTile.TargetTempo, UDanceUtilsFunctionLibrary::GetAcceptanceRate(), true) || bBypassOutOfTempo)
	{
		DanceCharacter->MoveTo(NextTile.Position, CurrentTile.TargetTempo * SongTempo->GetFrequency() * 0.95f);
		SectionManager->SectionChanged(NextTile.Section);
	}
	else
	{
		DanceCharacter->MoveFailed.Broadcast();
	}
}

void AChoreoPlayerController::OnFinishedMovement()
{
	
}

void AChoreoPlayerController::OnPlayerDied()
{
	LevelProgress->LoadCheckpoint();
	DanceCharacter->StopMovement();
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




