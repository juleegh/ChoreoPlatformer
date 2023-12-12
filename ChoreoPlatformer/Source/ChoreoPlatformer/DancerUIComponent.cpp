// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "DancerHealthComponent.h"
#include "ChoreoPlayerController.h"
#include "InventoryComponent.h"
#include "SongTempoComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "TilemapLevelManager.h"

const FGameplayTag UGameUI::LevelSelection = FGameplayTag::RequestGameplayTag("GameUI.LevelSelection");
const FGameplayTag UGameUI::GameStats = FGameplayTag::RequestGameplayTag("GameUI.GameStats");

UDancerUIComponent::UDancerUIComponent()
{
    static ConstructorHelpers::FClassFinder<UCommonActivatableWidget> DanceWidget(TEXT("/Game/GameScreen/C_GameScreen"));
    if (DanceWidget.Succeeded())
    {
        GameUIClass = DanceWidget.Class;
    }

    PrimaryComponentTick.bCanEverTick = false;
}

void UDancerUIComponent::BeginPlay()
{
    Super::BeginPlay();

    GameUI = Cast<UGameUI>(CreateWidget<UCommonUserWidget>(GetWorld()->GetFirstPlayerController(), GameUIClass));
    GameUI->AddToViewport();
    GameUI->SetVisibility(ESlateVisibility::Visible);
}

void UGameUI::LoadMenu()
{
    ClearGameWidgets();
    PushMenuWidget(LevelSelectionClass, LevelSelection);
}

void UGameUI::LoadGame()
{
    ClearMenuWidgets();
    PushGameWidget(StatsClass, GameStats);
}

void UGameUI::UpdateCountdown(int TemposLeft)
{
    if (GameWidgets.Contains(GameStats))
    {
        auto DancerStats = Cast<UDancerStats>(GameWidgets[GameStats]);
        DancerStats->UpdateCountdown(TemposLeft);
    }
}

void UGameUI::PromptTempoResult(float Distance)
{
    if (GameWidgets.Contains(GameStats))
    {
        auto DancerStats = Cast<UDancerStats>(GameWidgets[GameStats]);
        DancerStats->PromptTempoResult(Distance);
    }
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

