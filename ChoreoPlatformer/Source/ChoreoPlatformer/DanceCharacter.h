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
	ADanceCharacter();

protected:
	UPROPERTY()
	class UMoveTimeline* MoveTimeline;
	virtual void BeginPlay() override;

private:

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveInDirection(FVector direction);
	void StopMovement();
	class AChoreoPlayerController* GetChoreoController() const;
};
