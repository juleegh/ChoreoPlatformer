// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Components/ActorComponent.h"
#include "GridElement.h"
#include "EventsDataAsset.h"
#include "DanceAudioManager.generated.h"

class USoundWave;

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ADanceAudioManager : public AActor
{
	GENERATED_BODY()

public:
	ADanceAudioManager();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAudioComponent* MetaSound;
	
	UPROPERTY(EditInstanceOnly, Category = "SoundFX")
	class USoundWave* Song;
	UPROPERTY(EditInstanceOnly, Category = "SoundFX")
	float SongBPM;
	UPROPERTY(EditDefaultsOnly, Category = "SoundFX")
	TMap<EMoveResult, FName> Triggers;
	UPROPERTY(EditDefaultsOnly, Category = "SoundFX")
	TMap<EMoveResult, class USoundWave*> Effects;

public:
	void InitializeSong();
	void MarkTempo();
	void ResetSong();
	void PlayMoveResult(EMoveResult MoveResult);
	void SetSuccessRate(float value);
};