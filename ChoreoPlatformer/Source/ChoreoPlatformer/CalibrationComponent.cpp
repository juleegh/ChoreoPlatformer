// Fill out your copyright notice in the Description page of Project Settings.


#include "CalibrationComponent.h"
#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "ChoreoLevelActor.h"
#include "DanceUtilsFunctionLibrary.h"

UCalibrationComponent::UCalibrationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UCalibrationComponent::IsCalibrated()
{
	return bIsCalibrated;
}

float UCalibrationComponent::GetCalibrationDelta()
{
	if (PostTempos == 0)
		return 0;
	return (PostTempoMargin / PostTempos);
}


void UCalibrationComponent::ReceiveInput()
{
	if (!SongTempo)
	{
		SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	}
	float Result = SongTempo->TempoResult(1);
	if (Result > UDanceUtilsFunctionLibrary::GetAcceptanceRate())
	{
		PostTempos++;
		PostTempoMargin += Result;
	}

	SongTempo->SetupCalibrationDeficit(GetCalibrationDelta());
	if (Result <= UDanceUtilsFunctionLibrary::GetAcceptanceRate())
	{
		Streak++;
	}
	else
	{
		Streak = 0;
	}

	if (Streak >= RequiredStreak)
	{
		bIsCalibrated = true;
	}
}

void UCalibrationComponent::Setup(int Required)
{
	RequiredStreak = Required;
	PostTempoMargin = 0;
}


ACalibrator::ACalibrator()
{
	Calibration = CreateDefaultSubobject<UCalibrationComponent>(TEXT("Calibration"));
}

void ACalibrator::BeginPlay()
{
	Super::BeginPlay();
	EnableInput(GetWorld()->GetFirstPlayerController());
	auto PlayerInputComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->InputComponent;
	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &ACalibrator::KeyPressed);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &ACalibrator::KeyPressed);
	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &ACalibrator::KeyPressed);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &ACalibrator::KeyPressed);
	Calibration->Setup(RequiredStreak);
}

void ACalibrator::KeyPressed()
{
	Calibration->ReceiveInput();
	if (Calibration->IsCalibrated())
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
		if (auto TutorialActor = Cast<ATutorialLevelActor>(GetWorld()->GetLevelScriptActor()))
		{
			TutorialActor->CallibrationEnded();
		}
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::OpenLevel(World, FName("Map_1"));
		}
	}
}