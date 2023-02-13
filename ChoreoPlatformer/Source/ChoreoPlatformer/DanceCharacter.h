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
	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation;
	UFUNCTION(BlueprintImplementableEvent)
	void MoveCharacterToLocation();

private:

public:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveInDirection(FVector direction);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Min;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Max;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Threshold;
	class AChoreoPlayerController* GetChoreoController();
};
