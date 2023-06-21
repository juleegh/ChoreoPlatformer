// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Components/ActorComponent.h"
#include "TimelineCreatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimelineEnded);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTimelineTick, float, val);

UCLASS()
class CHOREOPLATFORMER_API UTimelineCreatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimelineCreatorComponent();
    void Initialize();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void PlayTimeline();
    void Reset();
    void Stop(bool bForceFinishAction);
    bool IsRunning() const;
    UPROPERTY()
    FTimelineEnded TimelineEnded;
    void MoveToPosition(FVector TargetPosition, float TimelineLength);
    void RotateToPosition(FRotator TargetPosition, float TimelineLength);

protected:
    UPROPERTY()
    AActor* TimelineTarget;
    UPROPERTY()
    UTimelineComponent* MyTimeline;
    UPROPERTY()
    class UCurveFloat* FloatCurve;

    UPROPERTY()
    FVector OriginLocation;
    UPROPERTY()
    FVector TargetLocation;
    UPROPERTY()
    FRotator OriginRotation;
    UPROPERTY()
    FRotator TargetRotation;
    UPROPERTY()
    FTimelineTick TimelineTick;

    UFUNCTION()
    void TimelineCallback(float val);
    UFUNCTION()
    void MoveCallback(float val);
    UFUNCTION()
    void RotateCallback(float val);
    UFUNCTION()
    void TimelineFinishedCallback();

    UPROPERTY()
    TEnumAsByte<ETimelineDirection::Type> TimelineDirection;
    UPROPERTY()
    float ElapsedTime;
};