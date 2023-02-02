// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"

// Sets default values
ADanceCharacter::ADanceCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = GetActorLocation();
}

// Called every frame
void ADanceCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADanceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto controller = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());

	PlayerInputComponent->BindAction("Up", IE_Pressed, controller, &AChoreoPlayerController::PressedUp);
	PlayerInputComponent->BindAction("Down", IE_Pressed, controller, &AChoreoPlayerController::PressedDown);
	PlayerInputComponent->BindAction("Left", IE_Pressed, controller, &AChoreoPlayerController::PressedLeft);
	PlayerInputComponent->BindAction("Right", IE_Pressed, controller, &AChoreoPlayerController::PressedRight);
}

void ADanceCharacter::MoveInDirection(FVector direction)
{
	TargetLocation = GetActorLocation() + direction * 100;
	MoveCharacterToLocation();
}


