// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Components/ActorComponent.h"
#include "TimelineCreatorComponent.generated.h"


UCLASS()
class CHOREOPLATFORMER_API UTimelineCreatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimelineCreatorComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void PlayTimeline();
    void Stop();
    bool IsRunning() const;

protected:
    UPROPERTY()
    AActor* TimelineTarget;
    UPROPERTY()
    UTimelineComponent* MyTimeline;
    UPROPERTY()
    class UCurveFloat* FloatCurve;

    virtual void BeginPlay() override;
    UFUNCTION()
    virtual void TimelineCallback(float val) {}
    UFUNCTION()
    virtual void TimelineFinishedCallback() {}

    UPROPERTY()
    TEnumAsByte<ETimelineDirection::Type> TimelineDirection;
    UPROPERTY()
    float TimelineLength = 0.15f;
    UPROPERTY()
    float ElapsedTime;
};

UCLASS()
class CHOREOPLATFORMER_API UMoveTimeline : public UTimelineCreatorComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    FVector OriginLocation;
    UPROPERTY()
    FVector TargetLocation;
    void TimelineCallback(float val) override;

public:
    void MoveToPosition(FVector TargetPosition);
};

UCLASS()
class CHOREOPLATFORMER_API URotateTimeline : public UTimelineCreatorComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    FRotator OriginRotation;
    UPROPERTY()
    FRotator TargetRotation;
    void TimelineCallback(float val) override;

public:
    void RotateToPosition(FRotator TargetPosition);
};
