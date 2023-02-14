// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "TimelineCreatorComponent.h"

ADanceCharacter::ADanceCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MoveTimeline = CreateDefaultSubobject<UMoveTimeline>("Move Timeline");
}

void ADanceCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADanceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto controller = GetChoreoController();

	PlayerInputComponent->BindAction("Up", IE_Pressed, controller, &AChoreoPlayerController::PressedUp);
	PlayerInputComponent->BindAction("Down", IE_Pressed, controller, &AChoreoPlayerController::PressedDown);
	PlayerInputComponent->BindAction("Left", IE_Pressed, controller, &AChoreoPlayerController::PressedLeft);
	PlayerInputComponent->BindAction("Right", IE_Pressed, controller, &AChoreoPlayerController::PressedRight);
}

void ADanceCharacter::MoveInDirection(FVector direction)
{
	if (MoveTimeline->IsRunning())
	{
		return;
	}
	MoveTimeline->MoveToPosition(GetActorLocation() + direction * 100);
}

AChoreoPlayerController* ADanceCharacter::GetChoreoController()
{
	return Cast<AChoreoPlayerController>(GetController());
}



