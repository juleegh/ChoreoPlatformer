// Fill out your copyright notice in the Description page of Project Settings.

#include "DancerUIComponent.h"
#include "ChoreoPlayerController.h"
#include "ComponentGetters.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"

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
    const FGameplayTag GTMainMenu = FGameplayTag::RequestGameplayTag("GameUI.MainMenu");
    PushMenuWidget(WidgetClasses[GTMainMenu], GTMainMenu);
}

void UGameUI::LoadGame()
{
    ClearMenuWidgets();
    MenuWidgets.Empty();
    const FGameplayTag GTGameStats = FGameplayTag::RequestGameplayTag("GameUI.GameStats");
    PushGameWidget(WidgetClasses[GTGameStats], GTGameStats);
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
    const FGameplayTag GTGameStats = FGameplayTag::RequestGameplayTag("GameUI.GameStats");
    if (GameWidgets.Contains(GTGameStats))
    {
        auto DancerStats = Cast<UDancerStats>(GameWidgets[GTGameStats]);
        DancerStats->UpdateCountdown(TemposLeft);
    }
}

void UGameUI::PromptTempoResult(EMoveResult MoveResult, bool AnimationType)
{
    const FGameplayTag GTGameStats = FGameplayTag::RequestGameplayTag("GameUI.GameStats");
    if (GameWidgets.Contains(GTGameStats))
    {
        auto DancerStats = Cast<UDancerStats>(GameWidgets[GTGameStats]);
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
    const FGameplayTag GTEndOfLevel = FGameplayTag::RequestGameplayTag("GameUI.EndOfLevel");
    ComponentGetters::GetLevelEventsComponent(GetWorld())->ActivateTrigger(LevelEndTrigger);
    ComponentGetters::GetSectionLevelManager(GetWorld())->NextSectionStart();
    ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->RemoveWidgetFromPile(GTEndOfLevel);
}

void UCollectablesUI::LoadOwnedItems()
{
    TArray<AActor*> PigeonModels;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), PigeonModels);

    for (auto Pigeon : PigeonModels)
    {
        if (auto Mesh = Cast<USkeletalMeshComponent>(Pigeon->GetComponentByClass(USkeletalMeshComponent::StaticClass())))
        {
            SkeletalMesh = Mesh;
            break;
        }
    }

    TArray<FClothingItemInfo*> Items;
    ItemsData->GetAllRows<FClothingItemInfo>(TEXT("ContextString"), Items);
    
    for (auto ItemInfo : Items)
    {
        OwnedItems.Add(ItemInfo->Identifier, HasCollectedItem(ItemInfo->Identifier));

        if (!CurrentOutfit.Contains(ItemInfo->BodySocket))
        {
            auto ClothingActor = GetWorld()->SpawnActor<AActor>(ClothesBP);
            FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
            TransformRules.ScaleRule = EAttachmentRule::KeepWorld;
            ClothingActor->AttachToComponent(SkeletalMesh, TransformRules, ItemInfo->BodySocket.GetTagName());
            CurrentOutfit.Add(ItemInfo->BodySocket, ClothingActor);
        }
    }

}

TMap<FGameplayTag, bool> UCollectablesUI::GetItems()
{
    if (OwnedItems.IsEmpty())
    {
        LoadOwnedItems();
    }

    return OwnedItems;
}

FClothingItemInfo& UCollectablesUI::GetClothingItem(FGameplayTag ItemType)
{
    return *ComponentGetters::GetInventoryComponent(GetWorld())->GetClothingInfo(ItemType);
}


bool UCollectablesUI::HasCollectedItem(FGameplayTag ItemType)
{
    return ComponentGetters::GetInventoryComponent(GetWorld())->HasCollectedItem(ItemType);
}

void UCollectablesUI::PutOnItem(FGameplayTag ItemType)
{
    auto ItemInfo = GetClothingItem(ItemType);
    if (!HasCollectedItem(ItemInfo.Identifier))
    {
        return;
    }

    AActor* ClothingActor = CurrentOutfit[ItemInfo.BodySocket];
    auto Clothing = Cast<UStaticMeshComponent>(ClothingActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

    if (Clothing->GetStaticMesh() == ItemInfo.Mesh)
    {
        Clothing->SetStaticMesh(nullptr);
        return;
    }

    Clothing->SetStaticMesh(ItemInfo.Mesh);
}

