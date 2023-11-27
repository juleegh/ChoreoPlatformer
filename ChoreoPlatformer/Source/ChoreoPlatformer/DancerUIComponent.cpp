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
        DancerClass = Stats.Class;
    }

    PrimaryComponentTick.bCanEverTick = false;
}

void UDancerUIComponent::BeginPlay()
{
    Super::BeginPlay();

    DancerUI = Cast<UDancerUI>(CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), DancerClass));
    DancerUI->AddToViewport();
    DancerUI->SetVisibility(ESlateVisibility::Visible);
    DancerUI->TempoComponent = UDanceUtilsFunctionLibrary::GetSongTempoComponent(GetOwner());
    DancerUI->InitializeEvents();
}

void UDancerUIComponent::UpdateCountdown(int TemposLeft)
{
    DancerUI->UpdateCountdown(TemposLeft);
}

void UDancerUIComponent::PromptTempoResult(float Distance)
{
    DancerUI->PromptTempoResult(Distance);
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

