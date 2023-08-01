// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DanceCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerMoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerNewPosition);

UCLASS()
class CHOREOPLATFORMER_API ADanceCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADanceCharacter();

protected:
	UPROPERTY()
	class UTimelineCreatorComponent* MoveTimeline;
	virtual void BeginPlay() override;
	UFUNCTION()
	void ReachedNextTile();

private:

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerMoved PlayerMoved;
	UPROPERTY(BlueprintAssignable)
	FMoveFailed MoveFailed;
	UPROPERTY(BlueprintAssignable)
	FPlayerNewPosition PlayerNewPosition;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveTo(FVector position, float Duration);
	void StopMovement();
	class AChoreoPlayerController* GetChoreoController() const;
	class UTimelineCreatorComponent* GetMovementTimeline() { return MoveTimeline; }
	UFUNCTION(BlueprintImplementableEvent)
	void SetupToLevel();
};
