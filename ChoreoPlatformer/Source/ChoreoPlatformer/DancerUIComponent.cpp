// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "ChoreoPlayerController.h"
#include "ComponentGetters.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

const FGameplayTag UGameUI::LevelSelection = FGameplayTag::RequestGameplayTag("GameUI.LevelSelection");
const FGameplayTag UGameUI::GameStats = FGameplayTag::RequestGameplayTag("GameUI.GameStats");

UDancerUIComponent::UDancerUIComponent()
{
    static ConstructorHelpers::FClassFinder<UCommonActivatableWidget> DanceWidgetClass(TEXT("/Game/Widgets/C_GameScreen"));
    if (DanceWidgetClass.Succeeded())
    {
        GameUIClass = DanceWidgetClass.Class;
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

void ULevelSelectionUI::ChangedLevelSelected(int index)
{
    LevelIndex = index;
}

void ULevelSelectionUI::LoadSelected()
{
    FName LevelName = FName("Level.AncientCity." + FString::FromInt(LevelIndex));
    Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GoToLevel(FGameplayTag::RequestGameplayTag(LevelName));
}

int ULevelCompleteUI::GetStepsByAccuracy(ETempoAccuracy Accuracy)
{
    return ComponentGetters::GetDancerHealthComponent(GetWorld())->GetStepsByAccuracy(Accuracy);
}

void ULevelCompleteUI::GoToNextSection()
{
    ComponentGetters::GetLevelEventsComponent(GetWorld())->ActivateTrigger(LevelEndTrigger);
    ComponentGetters::GetSectionLevelManager(GetWorld())->NextSectionStart();
}

