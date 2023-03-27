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
	int Pre = PreTempos == 0 ? 1 : PreTempos;
	int Post = PostTempos == 0 ? 1 : PostTempos;
	return (PreTempoMargin / PreTempos) - (PostTempoMargin / PostTempos);
}


void UCalibrationComponent::ReceiveInput()
{
	if (!SongTempo)
	{
		SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	}
	float Result = SongTempo->TempoPercentage();
	if (Result < 0.5f)
	{
		PostTempos++;
		PostTempoMargin += Result;
	}
	else
	{
		PreTempos++;
		PreTempoMargin += 1 - Result;
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