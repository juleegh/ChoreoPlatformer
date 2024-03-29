// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ChoreoPlayerController.h"
#include "TimelineCreatorComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"
#include "DanceAudioManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ADanceCharacter::ADanceCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
	ColorTimeline = CreateDefaultSubobject<UColorTimelineComponent>("Color Timeline");
}

void ADanceCharacter::BeginPlay()
{
	Super::BeginPlay();
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

void ADanceCharacter::InitializeToLevel(float Tempo)
{
	GetMesh()->SetPlayRate(Tempo);
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
	DanceAudio->PlayMoveResult(MoveResult);
}

void ADanceCharacter::SetupPlayerCamera(FGameplayTag CameraStyle)
{
	FPigeonCameraSettings CameraSettings = FlavorCameraSettings[FGameplayTag::RequestGameplayTag(FName("CameraSettings.Default"))];
	if (FlavorCameraSettings.Contains(CameraStyle))
	{
		CameraSettings = FlavorCameraSettings[CameraStyle];
	}

	GetMesh()->SetRelativeRotation(CameraSettings.PigeonRotation);
	if (CameraSettings.bForcePigeonLocation)
	{
		SetActorLocation(CameraSettings.PigeonWorldLocation);
	}
	if (UCameraComponent* PigeonCamera = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass())))
	{
		PigeonCamera->SetRelativeRotation(CameraSettings.CameraRotation);
	}
	
	if (USpringArmComponent* SpringArm = Cast<USpringArmComponent>(GetComponentByClass(USpringArmComponent::StaticClass())))
	{
		SpringArm->TargetOffset = CameraSettings.TargetOffset;
		SpringArm->TargetArmLength = CameraSettings.TargetArmLength;
		SpringArm->SetRelativeRotation(CameraSettings.ArmRotation);
	}

}
