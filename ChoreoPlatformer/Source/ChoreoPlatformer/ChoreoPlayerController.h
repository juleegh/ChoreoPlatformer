// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChoreoPlayerController.generated.h"

/**
 *
 */
UCLASS()
class CHOREOPLATFORMER_API AChoreoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY()
	class USongTempoController* SongTempo;

public:
	AChoreoPlayerController();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class USongTempoController* GetSongTempoController() { return SongTempo; }
};
