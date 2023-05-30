// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "DancerHealthComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UDancerUIComponent::UDancerUIComponent()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> Stats(TEXT("/Game/Widgets/C_MainStats"));
    if (Stats.Succeeded())
    {
        DancerClass = Stats.Class;
    }

    static ConstructorHelpers::FClassFinder<UUserWidget> Challenge(TEXT("/Game/Widgets/C_Challenges"));
    if (Challenge.Succeeded())
    {
        ChallengeClass = Challenge.Class;
    }
    PrimaryComponentTick.bCanEverTick = false;
}

void UDancerUIComponent::BeginPlay()
{
    Super::BeginPlay();
    DancerUI = Cast<UDancerUI>(CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), DancerClass));
    DancerUI->AddToViewport();
    DancerUI->SetVisibility(ESlateVisibility::Visible);

    ChallengeUI = Cast<UChallengeUI>(CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), ChallengeClass));
    ChallengeUI->AddToViewport();
    ChallengeUI->SetVisibility(ESlateVisibility::Visible);
}

void UDancerUIComponent::UpdateHealth(float Current, float Max)
{
    StateUI.CurrentHealth = Current;
    StateUI.MaxHealth = Max;
    DancerUI->UpdateUIState(StateUI);
}

void UDancerUIComponent::UpdateCountdown(int TemposLeft)
{
    DancerUI->UpdateCountdown(TemposLeft);
}

void UDancerUIComponent::PromptTempoResult(float Distance)
{
    DancerUI->PromptTempoResult(Distance);
}

void UDancerUIComponent::ChallengeStarted(EChallengeType ChallengeType)
{
    ChallengeUI->ChallengeStarted(ChallengeType);
}

void UDancerUIComponent::ChallengeEnded(EChallengeType ChallengeType, bool Success)
{
    ChallengeUI->ChallengeEnded(ChallengeType, Success);
}

int ULevelCompleteUI::GetPerfectSteps()
{
    return UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->GetStepsByAccuracy(ETempoAccuracy::Perfect);
}

int ULevelCompleteUI::GetGoodSteps()
{
    return UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->GetStepsByAccuracy(ETempoAccuracy::Great);
}

int ULevelCompleteUI::GetBadSteps()
{
    return UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->GetStepsByAccuracy(ETempoAccuracy::Bad);
}
