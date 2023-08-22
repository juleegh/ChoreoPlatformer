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
    DancerUI->TempoComponent = UDanceUtilsFunctionLibrary::GetSongTempoComponent(GetOwner());
    DancerUI->InitializeEvents();

    ChallengeUI = Cast<UChallengeUI>(CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), ChallengeClass));
    ChallengeUI->AddToViewport();
    ChallengeUI->SetVisibility(ESlateVisibility::Visible);
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

USongTempoComponent* UDancerUI::GetTempoComponent()
{
    return TempoComponent;
}

int ULevelCompleteUI::GetStepsByAccuracy(ETempoAccuracy Accuracy)
{
    return UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->GetStepsByAccuracy(Accuracy);
}

FText ULevelCompleteUI::GetChallengeResultByType(EChallengeType ChallengeType)
{
    int Collected = UDanceUtilsFunctionLibrary::GetTilemapLevelManager(GetWorld())->GetCollectedByChallengeType(ChallengeType);
    int Total = UDanceUtilsFunctionLibrary::GetTilemapLevelManager(GetWorld())->GetTotalByChallengeType(ChallengeType);
    return FText::Format(FText::FromString(TEXT("{0}/{1}")), FText::AsNumber(Collected), FText::AsNumber(Total));
}
