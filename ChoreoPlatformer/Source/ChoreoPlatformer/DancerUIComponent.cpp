// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "Kismet/GameplayStatics.h"

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
}

void UDancerUIComponent::UpdateHealth(int Current, int Max)
{
    StateUI.CurrentHealth = Current;
    StateUI.MaxHealth = Max;
    DancerUI->UpdateUIState(StateUI);
}