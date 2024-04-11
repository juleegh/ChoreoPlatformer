// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "CalibrationComponent.h"
#include "GridCell.h"
#include "TimelineCreatorComponent.h"
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
	return !UGameplayStatics::GetCurrentLevelName(this).Equals("GameIntro");
}

bool AChoreoPlayerController::InEndlessMode()
{
	return UGameplayStatics::GetCurrentLevelName(this).Equals("EndlessMode");
}

bool AChoreoPlayerController::IsPaused()
{
	const FGameplayTag GTPause = FGameplayTag::RequestGameplayTag("GameUI.Pause");
	return DancerUI->GetGameUI()->IsScreenActive(GTPause);
}

void AChoreoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DanceCharacter = Cast<ADanceCharacter>(GetPawn());
	DancerHealth->PlayerDied.AddDynamic(this, &AChoreoPlayerController::OnPlayerDied);
	SongTempo->TempoCountdown.AddDynamic(DancerUI->GetGameUI(), &UGameUI::UpdateCountdown);

	if (InGame())
	{
		DancerUI->GetGameUI()->LoadGame();
		CheckForCalibration();
	}
	else
	{
		DancerUI->GetGameUI()->LoadMenu();
	}
	EndlessMode = ComponentGetters::GetEndlessLevelManager(GetWorld());
}

void AChoreoPlayerController::CheckForCalibration()
{
#if WITH_EDITOR
	if (!bBypassCalibration)
#endif
		TriggerCalibration();
}

void AChoreoPlayerController::GoToLevel(const FGameplayTag Level)
{
	auto SectionManager = ComponentGetters::GetSectionLevelManager(GetWorld());
	
	if (!InGame())
	{
		FString LevelName = Level.RequestDirectParent().GetTagName().ToString();
		LevelName.RemoveFromStart("Level.");
		
		if (auto GameInstance = Cast<UChoreoGameInstance>(GetGameInstance()))
		{
			GameInstance->CurrentLevel = Level;
		}
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
	}
	else
	{
		if (!SectionManager)
		{
			return;
		}
		if (SectionManager->GetCurrentSection().RequestDirectParent().MatchesTag(Level.RequestDirectParent()))
		{
			DancerUI->GetGameUI()->CancelMenu();
			ComponentGetters::GetSectionLevelManager(GetWorld())->StartFromSection(Level);
		}
		else
		{
			FString LevelName = Level.RequestDirectParent().GetTagName().ToString();
			LevelName.RemoveFromStart("Level.");
			if (auto GameInstance = Cast<UChoreoGameInstance>(GetGameInstance()))
			{
				GameInstance->CurrentLevel = Level;
			}
			UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
		}
	}
}

void AChoreoPlayerController::GoBackToMainMenu()
{
	if (!InGame())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FString LevelName = "GameIntro";
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
	}
}

void AChoreoPlayerController::TogglePause()
{
	if (!IsPaused())
	{
		const FGameplayTag GTPause = FGameplayTag::RequestGameplayTag("GameUI.Pause");
		DancerUI->GetGameUI()->GoToGameScreen(GTPause);
	}
	else
	{
		DancerUI->GetGameUI()->CancelMenu();
	}
}

bool AChoreoPlayerController::CanMove()
{
	const FGameplayTag GTEOL = FGameplayTag::RequestGameplayTag("GameUI.EndOfLevel");
	if (!InGame() || IsPaused() || DancerUI->GetGameUI()->IsScreenActive(GTEOL) || bIsDead)
	{
		return false;
	}
	return true;
}

void AChoreoPlayerController::Move(const FInputActionValue& Value)
{
	FVector Direction = Value.Get<FVector>();
	if (Direction.X != 0 && Direction.Y != 0)
	{
		Direction.Y = 0;
	}

	if (CanMove())
	{
		CheckMovement(Direction);
	}
}

void AChoreoPlayerController::PauseGame(const FInputActionValue& Value)
{
	const FGameplayTag GTEOL = FGameplayTag::RequestGameplayTag("GameUI.EndOfLevel");
	if (!InGame() || DancerUI->GetGameUI()->IsScreenActive(GTEOL))
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
		ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::Calibrating);
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

	bool bCantPass = CurrentTile.bForcesDirection && CurrentTile.ForcedDirection != Direction;
#if WITH_EDITOR
	bCantPass = bCantPass && !bBypassObstacles;
#endif

	if (bCantPass)
	{
		DancerUI->GetGameUI()->PromptTempoResult(EMoveResult::InvalidDirection, false);
		ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::InvalidDirection);
		return;
	}

	bool bHitElement = NextTile.bHitElement;
#if WITH_EDITOR
	bHitElement = bHitElement && !bBypassObstacles;
#endif

	if (bHitElement)
	{
		auto Interaction = NextTile.HitElement->TriggerInteraction();
		DancerUI->GetGameUI()->PromptTempoResult(Interaction, Interaction == EMoveResult::ActionCompleted);

		if (Interaction == EMoveResult::Blocked)
		{
			ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::Blocked);
		}
		return;
	}

	if (!NextTile.HitCell)
	{
		return;
	}

	DancerHealth->CountStep(UDanceUtilsFunctionLibrary::GetTempoResult(Result));
	EMoveResult MoveResult = CurrentTile.TargetTempo >= 1 ? EMoveResult::Black_OK : EMoveResult::Half_OK;

	bool bIsOnTempo = SongTempo->IsOnTempo(CurrentTile.TargetTempo, UDanceUtilsFunctionLibrary::GetAcceptanceRate(), true);
#if WITH_EDITOR
	bIsOnTempo = bIsOnTempo || bBypassOutOfTempo;
#endif

	if (bIsOnTempo)
	{
		if (InEndlessMode())
		{
			if (EndlessMode->ShouldShuffleWorldInstead(NextTile.Position))
			{
				DanceCharacter->RotateTowards(NextTile.Position);
				EndlessMode->ShuffleWorldDown(CurrentTile.TargetTempo * ComponentGetters::GetSongTempoComponent(GetWorld())->GetFrequency() * 0.95f);
			}
			else
			{
				EndlessMode->PlayerMoved(Direction);
				DanceCharacter->MoveTo(NextTile.Position, CurrentTile.TargetTempo);
			}
		}
		else
		{
			DanceCharacter->MoveTo(NextTile.Position, CurrentTile.TargetTempo);
		}
		DancerUI->GetGameUI()->PromptTempoResult(MoveResult, true);
	}
	else
	{
		DancerUI->GetGameUI()->PromptTempoResult(EMoveResult::Bad, false);
		MoveResult = EMoveResult::Bad;
	}
	DanceCharacter->ToggleReaction(MoveResult);
	ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(MoveResult);
}

void AChoreoPlayerController::OnPlayerDied()
{
	DanceCharacter->StopMovement();

	float DelayDuration = 2.0f;
	FTimerDelegate TimerCallback;
	TimerCallback.BindUFunction(this, FName("RespawnPlayer"));
	bIsDead = true;
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, TimerCallback, DelayDuration, false);
}

bool AChoreoPlayerController::ShouldTakeDamage()
{
#if WITH_EDITOR
	return bShouldTakeDamage && !bIsDead;
#endif
	return !bIsDead;
}

void AChoreoPlayerController::RespawnPlayer()
{
	bIsDead = false;
	LevelProgress->LoadCheckpoint();
	if (DanceCharacter->PlayerNewPosition.IsBound())
	{
		DanceCharacter->PlayerNewPosition.Broadcast();
	}
}

void AChoreoPlayerController::TriggerCalibration()
{
	if (IsPaused())
	{
		TogglePause();
	}
	Calibration->StartCalibration();
	const FGameplayTag GTCalibration = FGameplayTag::RequestGameplayTag("GameUI.CalibrationScreen");
	DancerUI->GetGameUI()->GoToGameScreen(GTCalibration);
}

void AChoreoPlayerController::FinishCalibration()
{
	if (!Calibration->IsCalibrated())
	{
		return;
	}
	CalibrationEnded.Broadcast();
	ComponentGetters::GetDanceAudioManager(GetWorld())->ResetSong();
	DancerUI->GetGameUI()->CancelMenu();
}