// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DanceCharacter.generated.h"

UCLASS()
class CHOREOPLATFORMER_API ADanceCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADanceCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY()
	class UMoveTimeline* MoveTimeline;

private:

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveInDirection(FVector direction);
	class AChoreoPlayerController* GetChoreoController();
};
