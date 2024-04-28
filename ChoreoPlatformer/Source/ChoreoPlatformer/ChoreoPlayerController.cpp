// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoPlayerController.h"
#include "CalibrationComponent.h"
#include "GridCell.h"
#include "TimelineCreatorComponent.h"
#include "DancerUIComponent.h"
#include "DancerHealthComponent.h"
#include "DanceCharacter.h"
#include "SongTempoComponent.h"
#include "DanceAudioManager.h"
#include "TilemapLevelManager.h"
#include "ContextualElement.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DanceUtilsFunctionLibrary.h"
#include "LevelProgressComponent.h"
#include "ChoreoGameInstance.h"
#include "ComponentGetters.h"
#include "InventoryComponent.h"

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

bool AChoreoPlayerController::IsAlive()
{
	return !bIsDead;
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
		Calibration->TriggerCalibration();
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

bool AChoreoPlayerController::IsOnTempo()
{
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, DanceCharacter->GetActorLocation());
	bool bIsOnTempo = SongTempo->IsOnTempo(CurrentTile.TargetTempo, UDanceUtilsFunctionLibrary::GetAcceptanceRate(), true);
#if WITH_EDITOR
	bIsOnTempo = bIsOnTempo || bBypassOutOfTempo;
#endif
	return bIsOnTempo;
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

bool AChoreoPlayerController::TryInteraction()
{
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, DanceCharacter->GetActorLocation());
	FVector TargetPosition = UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), DanceCharacter->GetLastInput());
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, TargetPosition);

	bool bCantPass = CurrentTile.bForcesDirection && CurrentTile.ForcedDirection != DanceCharacter->GetLastInput();
#if WITH_EDITOR
	bCantPass = bCantPass && !bBypassObstacles;
#endif

	if (bCantPass)
	{
		DancerUI->GetGameUI()->PromptTempoResult(EMoveResult::InvalidDirection, false);
		ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::InvalidDirection);
		return true;
	}

	bool bHitElement = NextTile.bHitElement;
#if WITH_EDITOR
	bHitElement = bHitElement && !bBypassObstacles;
#endif

	if (!bHitElement)
	{
		return false;
	}

	EMoveResult MoveResult = CurrentTile.TargetTempo >= 1 ? EMoveResult::Black_OK : EMoveResult::Half_OK;
	if (IsOnTempo())
	{
		auto Interaction = NextTile.HitElement->TriggerInteraction();
		DancerUI->GetGameUI()->PromptTempoResult(Interaction, Interaction == EMoveResult::ActionCompleted);
		if (Interaction == EMoveResult::Blocked)
		{
			ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::Blocked);
		}
	}
	else
	{
		DancerUI->GetGameUI()->PromptTempoResult(EMoveResult::Bad, false);
		DanceCharacter->ToggleReaction(EMoveResult::Bad);
		ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::Bad);
	}

	return true;
}

bool AChoreoPlayerController::TryMovement()
{
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, DanceCharacter->GetActorLocation());
	float Result = SongTempo->TempoResult(CurrentTile.TargetTempo, true);
	FVector TargetPosition = UDanceUtilsFunctionLibrary::GetTransformedPosition(DanceCharacter->GetActorLocation(), DanceCharacter->GetLastInput());
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, TargetPosition);

	if (!NextTile.HitCell)
	{
		return false;
	}

	DancerHealth->CountStep(UDanceUtilsFunctionLibrary::GetTempoResult(Result));
	EMoveResult MoveResult = CurrentTile.TargetTempo >= 1 ? EMoveResult::Black_OK : EMoveResult::Half_OK;

	if (IsOnTempo())
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
				EndlessMode->PlayerMoved(DanceCharacter->GetLastInput());
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
	return true;
}