// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "ChoreoPlayerController.h"
#include "ComponentGetters.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

const FGameplayTag UGameUI::MainMenu = FGameplayTag::RequestGameplayTag("GameUI.MainMenu");
const FGameplayTag UGameUI::LevelSelection = FGameplayTag::RequestGameplayTag("GameUI.LevelSelection");
const FGameplayTag UGameUI::GameStats = FGameplayTag::RequestGameplayTag("GameUI.GameStats");
const FGameplayTag UGameUI::CollectablesScreen = FGameplayTag::RequestGameplayTag("GameUI.CollectablesScreen");
const FGameplayTag UGameUI::Pause = FGameplayTag::RequestGameplayTag("GameUI.Pause");
const FGameplayTag UGameUI::CalibrationScreen = FGameplayTag::RequestGameplayTag("GameUI.CalibrationScreen");
const FGameplayTag UGameUI::EndOfLevel = FGameplayTag::RequestGameplayTag("GameUI.EndOfLevel");

UDancerUIComponent::UDancerUIComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

UDancerUIComponent* UChoreoActivatableWidget::GetDancerUIComponent()
{
    return ComponentGetters::GetDancerUIComponent(GetWorld());
}

AChoreoPlayerController* UChoreoActivatableWidget::GetController()
{
    return Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
}

AChoreoPlayerController* UGameUI::GetController()
{
    return Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
}

void UChoreoActivatableWidget::SetSelected(UChoreoButtonBase* NewSelected)
{
    SelectedButton = NewSelected;
}

UDancerUIComponent* UChoreoButtonBase::GetDancerUIComponent()
{
    return ComponentGetters::GetDancerUIComponent(GetWorld());
}

void UChoreoButtonBase::OnClicked()
{
    OnButtonBaseClicked.Broadcast(this);
}

void UDancerUIComponent::BeginPlay()
{
    Super::BeginPlay();

    GameUI = Cast<UGameUI>(CreateWidget<UCommonUserWidget>(GetWorld()->GetFirstPlayerController(), ComponentGetters::GetController(GetWorld())->GameUIClass));
    GameUI->AddToViewport();
    GameUI->SetVisibility(ESlateVisibility::Visible);
}

void UDancerUIComponent::Confirm(const FInputActionValue& Value)
{
    GameUI->ConfirmMenu();
}

void UDancerUIComponent::Cancel(const FInputActionValue& Value)
{
    GameUI->CancelMenu();
}

void UGameUI::LoadMenu()
{
    ClearGameWidgets();
    GameWidgets.Empty();
    PushMenuWidget(WidgetClasses[MainMenu], MainMenu);
}

void UGameUI::LoadGame()
{
    ClearMenuWidgets();
    MenuWidgets.Empty();
    PushGameWidget(WidgetClasses[GameStats], GameStats);
}

void UGameUI::GoToMenuScreen(const FGameplayTag MenuScreen)
{
    if (WidgetClasses.Contains(MenuScreen))
    {
        PushMenuWidget(WidgetClasses[MenuScreen], MenuScreen);
    }
}

void UGameUI::GoToGameScreen(const FGameplayTag GameScreen)
{
    if (WidgetClasses.Contains(GameScreen))
    {
        PushGameWidget(WidgetClasses[GameScreen], GameScreen);
    }
}

void UGameUI::RemoveWidgetFromPile(const FGameplayTag& Identifier)
{
    if (GameWidgets.Contains(Identifier))
    {
        RemoveWidget(GameWidgets[Identifier]);
        GameWidgets[Identifier] = nullptr;
    }
    if (MenuWidgets.Contains(Identifier))
    {
        RemoveWidget(MenuWidgets[Identifier]);
        MenuWidgets[Identifier] = nullptr;
    }
}

bool UGameUI::IsScreenActive(const FGameplayTag& Screen)
{
    return GameWidgets.Contains(Screen) && IsValid(GameWidgets[Screen]);
}

void UGameUI::ExitGame()
{
    FPlatformMisc::RequestExit(false);
}

void UGameUI::UpdateCountdown(int TemposLeft)
{
    if (GameWidgets.Contains(GameStats))
    {
        auto DancerStats = Cast<UDancerStats>(GameWidgets[GameStats]);
        DancerStats->UpdateCountdown(TemposLeft);
    }
}

void UGameUI::PromptTempoResult(EMoveResult MoveResult, bool AnimationType)
{
    if (GameWidgets.Contains(GameStats))
    {
        auto DancerStats = Cast<UDancerStats>(GameWidgets[GameStats]);
        DancerStats->PromptTempoResult(MoveResult, AnimationType);
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
    ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->RemoveWidgetFromPile(UGameUI::EndOfLevel);
}

