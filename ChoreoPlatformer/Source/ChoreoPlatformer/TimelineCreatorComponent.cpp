#include "TimelineCreatorComponent.h"
#include "Curves/CurveBase.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

UTimelineCreatorComponent::UTimelineCreatorComponent()
{
    static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve(TEXT("/Game/Curves/C_MyCurve"));
    if (Curve.Succeeded())
    {
        FloatCurve = Curve.Object;
    }
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UTimelineCreatorComponent::BeginPlay()
{
    Super::BeginPlay();
    FOnTimelineEventStatic onTimelineFinishedCallback;
    FOnTimelineFloat onTimelineCallback;

    if (FloatCurve != NULL)
    {
        MyTimeline = NewObject<UTimelineComponent>(GetOwner(), FName("TimelineAnimation"));
        MyTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
        GetOwner()->BlueprintCreatedComponents.Add(MyTimeline); // Add to array so it gets saved
        MyTimeline->SetNetAddressable();    // This component has a stable name that can be referenced for replication

        MyTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
        MyTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

        MyTimeline->SetLooping(false);
        MyTimeline->SetTimelineLength(TimelineLength);
        MyTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

        MyTimeline->SetPlaybackPosition(0.0f, false);

        //Add the float curve to the timeline and connect it to your timelines's interpolation function
        onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
        onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
        MyTimeline->AddInterpFloat(FloatCurve, onTimelineCallback, "ElapsedTime");
        MyTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

        MyTimeline->RegisterComponent();
        TimelineTarget = GetOwner();
    }
}

void UTimelineCreatorComponent::TimelineFinishedCallback()
{
    if (TimelineEnded.IsBound())
    {
        TimelineEnded.Broadcast();
    }
}


void UTimelineCreatorComponent::Stop()
{
    if (MyTimeline != NULL)
    {
        MyTimeline->Stop();
    }
}

bool UTimelineCreatorComponent::IsRunning() const
{
    if (MyTimeline != NULL)
    {
        return MyTimeline->GetPlaybackPosition() < TimelineLength && MyTimeline->GetPlaybackPosition() > 0;
    }
    return false;
}


void UTimelineCreatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (MyTimeline != NULL)
    {
        MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
    }
}

void UTimelineCreatorComponent::PlayTimeline()
{
    if (MyTimeline != NULL)
    {
        MyTimeline->PlayFromStart();
    }
}

/*
void UMoveTimeline::TimelineFinishedCallback()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Did action")));
}
*/

void UMoveTimeline::MoveToPosition(FVector TargetPosition)
{
    OriginLocation = TimelineTarget->GetActorLocation();
    TargetLocation = TargetPosition;
    float Length = MyTimeline->GetTimelineLength();
    PlayTimeline();
}

void UMoveTimeline::TimelineCallback(float interpolatedVal)
{
    FVector Pos = UKismetMathLibrary::VLerp(OriginLocation, TargetLocation, MyTimeline->GetPlaybackPosition() / TimelineLength);
    TimelineTarget->SetActorLocation(Pos);
}

void URotateTimeline::RotateToPosition(FRotator TargetPosition)
{
    OriginRotation = TimelineTarget->GetActorRotation();
    TargetRotation = TargetPosition;
    float Length = MyTimeline->GetTimelineLength();
    PlayTimeline();
}

void URotateTimeline::TimelineCallback(float interpolatedVal)
{
    FRotator Rot = UKismetMathLibrary::RLerp(OriginRotation, TargetRotation, MyTimeline->GetPlaybackPosition() / TimelineLength, false);
    TimelineTarget->SetActorRotation(Rot);
}