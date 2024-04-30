// Fill out your copyright notice in the Description page of Project Settings.


#include "CalibrationComponent.h"
#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "DanceAudioManager.h"
#include "DancerUIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DanceUtilsFunctionLibrary.h"
#include "DanceCharacter.h"
#include "ComponentGetters.h"

UCalibrationComponent::UCalibrationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCalibrationComponent::TriggerCalibration()
{
	auto controller = ComponentGetters::GetController(GetWorld());
	if (controller->IsPaused())
	{
		controller->TogglePause();
	}
	
	StartCalibration();
	const FGameplayTag GTCalibration = FGameplayTag::RequestGameplayTag("GameUI.CalibrationScreen");
	ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->GoToGameScreen(GTCalibration);
}

void UCalibrationComponent::StartCalibration()
{
	bIsCalibrated = false;
	PostTempos = 0;
	PostTempoMargin = 0;
	Tries = 0;
}

bool UCalibrationComponent::IsCalibrated() const
{
	return bIsCalibrated;
}

float UCalibrationComponent::GetCalibrationDelta()
{
	if (PostTempos == 0)
		return 0;
	return (PostTempoMargin / PostTempos);
}

void UCalibrationComponent::ReceiveInput()
{
	if (!SongTempo)
	{
		SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	}
	float Percentage = SongTempo->TempoPercentage();
	PostTempos++;
	if (Percentage > 0.5f)
	{
		PostTempoMargin += 1 - Percentage;
	}
	else
	{
		PostTempoMargin -= Percentage;
	}

	Tries++;
	SongTempo->SetupCalibrationDeficit(GetCalibrationDelta());
	ComponentGetters::GetDanceCharacter(GetWorld())->ClearInput();
	ComponentGetters::GetDanceAudioManager(GetWorld())->PlayMoveResult(EMoveResult::Calibrating);
	Calibrating.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("Deviation: %f"), Percentage);
	if (SongTempo->IsOnTempo(1, UDanceUtilsFunctionLibrary::GetAcceptanceRate(), true) && Tries >= 8)
	{
		bIsCalibrated = true;
		CalibrationEnded.Broadcast();
		ComponentGetters::GetDanceAudioManager(GetWorld())->ResetSong();
		ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->CancelMenu();
	}
}
