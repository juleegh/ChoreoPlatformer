// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "ChoreoPlayerController.h"
#include "TimelineCreatorComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"
#include "DanceAudioManager.h"
#include "DancerUIComponent.h"
#include "SongTempoComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ADanceCharacter::ADanceCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
	ColorTimeline = CreateDefaultSubobject<UColorTimelineComponent>("Color Timeline");
	GameCamera = CreateDefaultSubobject<UGameCameraComponent>("Game Camera");
}

void ADanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentMovementSpeed = 0;
	MoveTimeline->Initialize();
	MoveTimeline->TimelineEnded.AddDynamic(this, &ADanceCharacter::ReachedNextTile);
	ColorTimeline->Initialize();
	ColorTimeline->AddMesh(GetMesh());

	if (APlayerController* ChoreoController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ChoreoController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(ChoreoIMC, 0);
		}
	}
	DanceAudio = ComponentGetters::GetDanceAudioManager(GetWorld());
}

void ADanceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto controller = ComponentGetters::GetController(GetWorld());
	auto UIcontroller = ComponentGetters::GetDancerUIComponent(GetWorld());
	if (auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, controller, &AChoreoPlayerController::Move);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, controller, &AChoreoPlayerController::PauseGame);
		EnhancedInputComponent->BindAction(ConfirmAction, ETriggerEvent::Triggered, UIcontroller, &UDancerUIComponent::Confirm);
		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Triggered, UIcontroller, &UDancerUIComponent::Cancel);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ADanceCharacter::MovePressed);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADanceCharacter::MoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ADanceCharacter::MoveReleased);

		EnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Started, this, &ADanceCharacter::MoveCamera);
		EnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Completed, this, &ADanceCharacter::StopCamera);
	}

}

void ADanceCharacter::InitializeToLevel(float Tempo)
{
	GetMesh()->SetPlayRate(1 / Tempo);
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

	CurrentMovementSpeed = Duration >= 1 ? 1 : -1 / Duration;
	PlayerMoved.Broadcast(Duration);
	RotateTowards(position);
	MoveTimeline->MoveToPosition(position, Duration * ComponentGetters::GetSongTempoComponent(GetWorld())->GetFrequency() * 0.95f);
}

void ADanceCharacter::RotateTowards(FVector position)
{
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), position);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	GetController()->SetControlRotation(Rotation);
}

void ADanceCharacter::StopMovement()
{
	MoveTimeline->Reset();
	MoveTimeline->Stop(true);
	CurrentMovementSpeed = 0;
}

void ADanceCharacter::Respawn(FVector NewPosition)
{
	SetActorLocation(NewPosition);
	MoveTimeline->Reset();
}

void ADanceCharacter::ReachedNextTile()
{
	CurrentMovementSpeed = 0;
	PlayerNewPosition.Broadcast();
}

void ADanceCharacter::ToggleReaction(EMoveResult MoveResult)
{
	if (ReactionColors.Contains(MoveResult))
	{
		ColorTimeline->ChangeColor(ReactionColors[MoveResult]);
		ColorTimeline->Blink();
	}
	if (ReactionParticles.Contains(MoveResult))
	{
		FTransform Transform = GetTransform();
		Transform.SetLocation(GetMesh()->GetSocketLocation(FName("root")));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ReactionParticles[MoveResult], Transform);
	}
	if (ReactionAnimations.Contains(MoveResult))
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(ReactionAnimations[MoveResult]);
	}
	if (!DanceAudio)
	{
		DanceAudio = ComponentGetters::GetDanceAudioManager(GetWorld());
	}
	DanceAudio->PlayMoveResult(MoveResult);
}

void ADanceCharacter::MoveReleased(const FInputActionValue& Value)
{
	InputDirection = FVector::Zero();
}

void ADanceCharacter::ClearInput()
{
	InputDirection = FVector::Zero();
}

void ADanceCharacter::MovePressed(const FInputActionValue& Value)
{
	InputDirection = Value.Get<FVector>();
	InputDirection.X = InputDirection.X == 0 ? 0 : InputDirection.X < 0 ? -1.0f : 1.0f;
	InputDirection.Y = InputDirection.Y == 0 ? 0 : InputDirection.Y < 0 ? -1.0f : 1.0f;

	if (InputDirection.X != 0 && InputDirection.Y != 0)
	{
		InputDirection.Y = 0;
	}
}

void ADanceCharacter::MoveTriggered(const FInputActionValue& Value)
{
	if (!GameCamera->IsMovingCamera())
	{
		return;
	}
	InputDirection = Value.Get<FVector>();
}

void ADanceCharacter::MoveCamera(const FInputActionValue& Value)
{
	GameCamera->ToggleCameraMovement(true);
}

void ADanceCharacter::StopCamera(const FInputActionValue& Value)
{
	InputDirection = FVector::Zero();
	GameCamera->ToggleCameraMovement(false);
}

FVector ADanceCharacter::GetCurrentInput() const
{
	return InputDirection;
}

float ADanceCharacter::GetMovementSpeed() const
{
	return CurrentMovementSpeed;
}