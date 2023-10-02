// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "TimelineCreatorComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

ADanceCharacter::ADanceCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
}

void ADanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	MoveTimeline->Initialize();
	MoveTimeline->TimelineEnded.AddDynamic(this, &ADanceCharacter::ReachedNextTile);
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

void ADanceCharacter::MoveTo(FVector position, float Duration)
{
	if (MoveTimeline->IsRunning())
	{
		MoveTimeline->Stop(true);
	}
	PlayerMoved.Broadcast();
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), position);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	GetController()->SetControlRotation(Rotation);
	MoveTimeline->MoveToPosition(position, Duration);
}

void ADanceCharacter::StopMovement()
{
	MoveTimeline->Reset();
	MoveTimeline->Stop(true);
}

void ADanceCharacter::ReachedNextTile()
{
	PlayerNewPosition.Broadcast();
}

AChoreoPlayerController* ADanceCharacter::GetChoreoController() const
{
	return Cast<AChoreoPlayerController>(GetController());
}



