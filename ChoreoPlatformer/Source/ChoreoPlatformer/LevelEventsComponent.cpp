#include "LevelEventsComponent.h"
#include "SongTempoComponent.h"
#include "Components/BoxComponent.h"
#include "ChoreoPlayerController.h"
#include "Kismet/GameplayStatics.h"

ULevelEventsComponent::ULevelEventsComponent()
{
    static ConstructorHelpers::FObjectFinder<UEventsDataAsset>DataAsset(TEXT("/Game/Events/LevelEvents"));
    if (DataAsset.Succeeded())
    {
        LevelEvents = DataAsset.Object;
    }
}

void ULevelEventsComponent::ActivateTrigger(FGameplayTag TriggerTag)
{
    if (LevelEvents->WidgetEvents.Contains(TriggerTag))
    {
        HandleWidgetEvent(TriggerTag);
    }
    if (LevelEvents->CountdownEvents.Contains(TriggerTag))
    {
        HandleCountdownEvent(TriggerTag);
    }
}

void ULevelEventsComponent::HandleWidgetEvent(FGameplayTag TriggerTag)
{
    auto EventInfo = LevelEvents->WidgetEvents[TriggerTag];
    if (EventInfo.bSpawnsWidget)
    {
        if (!Widgets.Contains(EventInfo.LevelWidget))
        {
            Widgets.Add(EventInfo.LevelWidget, CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), EventInfo.LevelWidget));
            Widgets[EventInfo.LevelWidget]->AddToViewport();
            Widgets[EventInfo.LevelWidget]->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        if (Widgets.Contains(EventInfo.LevelWidget))
        {
            Widgets[EventInfo.LevelWidget]->RemoveFromViewport();
            Widgets.Remove(EventInfo.LevelWidget);
        }
    }
}

void ULevelEventsComponent::HandleCountdownEvent(FGameplayTag TriggerTag)
{
    if (!Countdowns.Contains(TriggerTag))
    {
        auto SongTempo = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetSongTempoComponent();
        SongTempo->AddPauseTempos(LevelEvents->CountdownEvents[TriggerTag]);
    }
}

AEventTrigger::AEventTrigger()
{
    PrimaryActorTick.bCanEverTick = false;
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
    BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    RootComponent = BoxComponent;
}

void AEventTrigger::BeginPlay()
{
    Super::BeginPlay();
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEventTrigger::OnOverlapRangeBegin);
}

void AEventTrigger::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    auto LevelEvents = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetEventsComponent();
    LevelEvents->ActivateTrigger(ActorTrigger);
}
