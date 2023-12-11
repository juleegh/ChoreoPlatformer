// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "DancerHealthComponent.h"
#include "ChoreoPlayerController.h"
#include "InventoryComponent.h"
#include "SongTempoComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TilemapLevelManager.h"

UDancerUIComponent::UDancerUIComponent()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> Stats(TEXT("/Game/Widgets/C_MainStats"));
    if (Stats.Succeeded())
    {
        StatsClass = Stats.Class;
    }

    static ConstructorHelpers::FClassFinder<UUserWidget> Calibration(TEXT("/Game/Widgets/C_CalibrationScreen"));
    if (Calibration.Succeeded())
    {
        CalibrationClass = Calibration.Class;
    }

    PrimaryComponentTick.bCanEverTick = false;
}

void UDancerUIComponent::BeginPlay()
{
    Super::BeginPlay();

    DancerStats = Cast<UDancerStats>(CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), StatsClass));
    DancerStats->AddToViewport();
    DancerStats->SetVisibility(ESlateVisibility::Visible);
    DancerStats->TempoComponent = UDanceUtilsFunctionLibrary::GetSongTempoComponent(GetOwner());
    DancerStats->InitializeScreen();

    CalibrationScreen = Cast<UCalibrationScreen>(CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), CalibrationClass));
    CalibrationScreen->AddToViewport();
    CalibrationScreen->SetVisibility(ESlateVisibility::Visible);
    CalibrationScreen->TempoComponent = UDanceUtilsFunctionLibrary::GetSongTempoComponent(GetOwner());
    CalibrationScreen->InitializeScreen();
}

void UDancerUIComponent::UpdateCountdown(int TemposLeft)
{
    DancerStats->UpdateCountdown(TemposLeft);
}

void UDancerUIComponent::PromptTempoResult(float Distance)
{
    DancerStats->PromptTempoResult(Distance);
}

USongTempoComponent* UDancerUI::GetTempoComponent()
{
    return TempoComponent;
}

int ULevelCompleteUI::GetStepsByAccuracy(ETempoAccuracy Accuracy)
{
    return UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->GetStepsByAccuracy(Accuracy);
}

void ULevelCompleteUI::GoToNextSection()
{
    auto LevelEvents = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetEventsComponent();
    LevelEvents->ActivateTrigger(LevelEndTrigger);

    auto SectionManager = UDanceUtilsFunctionLibrary::GetSectionLevelManager(GetWorld());
    SectionManager->NextSectionStart();
}

