// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "TimelineCreatorComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"
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

	if (APlayerController* ChoreoController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ChoreoController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(ChoreoIMC, 0);
		}
	}
}

void ADanceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto controller = GetChoreoController();
	auto UIcontroller = ComponentGetters::GetDancerUIComponent(GetWorld());
	if (auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, controller, &AChoreoPlayerController::Move);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, controller, &AChoreoPlayerController::PauseGame);
		EnhancedInputComponent->BindAction(ConfirmAction, ETriggerEvent::Triggered, UIcontroller, &UDancerUIComponent::Confirm);
		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Triggered, UIcontroller, &UDancerUIComponent::Cancel);
	}

}

void ADanceCharacter::MoveTo(FVector position, float Duration)
{
	if (MoveTimeline->IsRunning())
	{
		MoveTimeline->Stop(true);
	}
	for (AActor* Adjacent : UDanceUtilsFunctionLibrary::GetAdjacent(this, GetActorLocation(), 1))
	{
		UDanceUtilsFunctionLibrary::ToggleHighlight(Adjacent, false);
	}
	for (AActor* Adjacent : UDanceUtilsFunctionLibrary::GetAdjacent(this, position, 1))
	{
		UDanceUtilsFunctionLibrary::ToggleHighlight(Adjacent, true);
	}

	PlayerMoved.Broadcast(Duration);
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), position);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	GetController()->SetControlRotation(Rotation);
	MoveTimeline->MoveToPosition(position, Duration * ComponentGetters::GetSongTempoComponent(GetWorld())->GetFrequency() * 0.95f);
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



