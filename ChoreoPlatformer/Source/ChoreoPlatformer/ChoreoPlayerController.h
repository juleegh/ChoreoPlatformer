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
	class USongTempoComponent* SongTempo;
	UPROPERTY()
	class UTileDetectorComponent* TileDetector;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;
	void CheckMovement(FVector Direction);

public:
	AChoreoPlayerController();
	class USongTempoComponent* GetSongTempoComponent() { return SongTempo; }
	void PressedUp();
	void PressedDown();
	void PressedLeft();
	void PressedRight();
	virtual void Tick(float DeltaTime) override;
};
