// Fill out your copyright notice in the Description page of Project Settings.

#include "DanceAudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "ComponentGetters.h"

ADanceAudioManager::ADanceAudioManager()
{
	MetaSound = CreateDefaultSubobject<UAudioComponent>(TEXT("MetaSound"));
}

void ADanceAudioManager::PlayMoveResult(EMoveResult MoveResult)
{
	if (Triggers.Contains(MoveResult))
	{
		MetaSound->SetTriggerParameter(Triggers[MoveResult]);
	}
	else if (Effects.Contains(MoveResult))
	{
		MetaSound->SetWaveParameter(FName("LastEffect"), Effects[MoveResult]);
		MetaSound->SetTriggerParameter(FName("SoundEffect"));
	}
}

void ADanceAudioManager::MarkTempo()
{
	MetaSound->SetTriggerParameter(FName("TempoStarted"));
}

void ADanceAudioManager::ResetSong()
{
	MetaSound->SetTriggerParameter(FName("StartSong"));
}

void ADanceAudioManager::InitializeSong()
{
	MetaSound->SetWaveParameter(FName("Song"), Song);
	MetaSound->SetFloatParameter(FName("SongBPM"), SongBPM);
	MetaSound->SetTriggerParameter(FName("StartSong"));
}

void ADanceAudioManager::SetSuccessRate(float value)
{
	MetaSound->SetFloatParameter(FName("SuccessRate"), value);
}

