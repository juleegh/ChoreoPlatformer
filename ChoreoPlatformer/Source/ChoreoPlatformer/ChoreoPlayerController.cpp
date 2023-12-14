// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "CalibrationComponent.h"
#include "GridCell.h"
#include "TimelineCreatorComponent.h"
#include "DancerUIComponent.h"
#include "ContextualElement.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ChoreoGameInstance.h"
#include "ComponentGetters.h"

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

bool AChoreoPlayerController::InGame()
{
	return ComponentGetters::GetSectionLevelManager(GetWorld()) != nullptr;
}

bool AChoreoPlayerController::IsPaused()
{
	return DancerUI->GetGameUI()->IsScreenActive(UGameUI::Pause);
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DanceCharacter = Cast<ADanceCharacter>(GetPawn());
	DancerHealth->PlayerDied.AddDynamic(this, &AChoreoPlayerController::OnPlayerDied);
	SongTempo->TempoCountdown.AddDynamic(DancerUI->GetGameUI(), &UGameUI::UpdateCountdown);

	if (!InGame())
	{
		DancerUI->GetGameUI()->LoadMenu();
	}
	else
	{
		DancerUI->GetGameUI()->LoadGame();
		/*
		if (bBypassCalibration)
		{
			CalibrationEnded.Broadcast();
			LevelEvents->ActivateTrigger(FGameplayTag::RequestGameplayTag(FName("tutorial.intro")));
		}
		*/
	}
}

void AChoreoPlayerController::GoToLevel(const FGameplayTag Level)
{
	UWorld* World = GetWorld();

	if (World)
	{
		FString LevelName = "C1_AncientCity";
		if (auto GameInstance = Cast<UChoreoGameInstance>(GetGameInstance()))
		{
			GameInstance->CurrentLevel = Level;
		}

		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
	}
}

void AChoreoPlayerController::TogglePause()
{
	if (!IsPaused())
	{
		DancerUI->GetGameUI()->GoToGameScreen(UGameUI::Pause);
	}
	else
	{
		DancerUI->GetGameUI()->CancelMenu();
	}
}

void AChoreoPlayerController::Move(const FInputActionValue& Value)
{
	if (!InGame() || IsPaused() || DancerUI->GetGameUI()->IsScreenActive(UGameUI::EndOfLevel) || bIsDead)
	{
		return;
	}

	CheckMovement(Value.Get<FVector>());
}
	
void AChoreoPlayerController::PauseGame(const FInputActionValue& Value)
{
	if (!InGame() || DancerUI->GetGameUI()->IsScreenActive(UGameUI::EndOfLevel))
	{
		return;
	}

	TogglePause();
}

void AChoreoPlayerController::CheckMovement(FVector Direction)
{
	if (!Calibration->IsCalibrated())
	{
		Calibration->ReceiveInput();
		Calibrating.Broadcast();
		return;
	}
	if (SongTempo->IsOnPause() || SongTempo->IsStopped())
	{
		return;
	}

	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, DanceCharacter->GetActorLocation());
	float Result = SongTempo->TempoResult(CurrentTile.TargetTempo, true);

	FVector TargetPosition = UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), Direction);
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, TargetPosition);

	if (CurrentTile.bForcesDirection && CurrentTile.ForcedDirection != Direction)
	{
		return;
	}

	if (NextTile.bHitElement)
	{
		NextTile.HitElement->TriggerInteraction();
		TriggerResultFeedback(Result);
		return;
	}

	if (!NextTile.HitCell || NextTile.TileType == ETempoTile::Blocker)
	{
		return;
	}

	TriggerResultFeedback(Result);

	if (SongTempo->IsOnTempo(CurrentTile.TargetTempo, UDanceUtilsFunctionLibrary::GetAcceptanceRate(), true) || bBypassOutOfTempo)
	{
		DanceCharacter->MoveTo(NextTile.Position, CurrentTile.TargetTempo);
	}
	else
	{
		DanceCharacter->MoveFailed.Broadcast();
	}
}

void AChoreoPlayerController::OnPlayerDied()
{
	DanceCharacter->StopMovement();

	float DelayDuration = 1.0f;
	FTimerDelegate TimerCallback;
	TimerCallback.BindUFunction(this, FName("RespawnPlayer"));
	bIsDead = true;
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, TimerCallback, DelayDuration, false);
	LevelProgress->LoadCheckpoint();
}

void AChoreoPlayerController::RespawnPlayer()
{
	bIsDead = false;
}

void AChoreoPlayerController::TriggerResultFeedback(float Result)
{
	DancerHealth->CountStep(UDanceUtilsFunctionLibrary::GetTempoResult(Result));
	DancerUI->GetGameUI()->PromptTempoResult(Result);
	ComponentGetters::GetSectionLevelManager(GetWorld())->PlayTempoResult(UDanceUtilsFunctionLibrary::GetTempoResult(Result));
}

void AChoreoPlayerController::TriggerCalibration()
{
	TogglePause();
	Calibration->StartCalibration();
	DancerUI->GetGameUI()->GoToGameScreen(UGameUI::CalibrationScreen);
}

void AChoreoPlayerController::FinishCalibration()
{
	if (!Calibration->IsCalibrated())
	{
		return;
	}
	CalibrationEnded.Broadcast();
	DancerUI->GetGameUI()->CancelMenu();
}