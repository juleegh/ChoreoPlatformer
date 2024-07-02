// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "DanceDefinitions.h"
#include "Components/ActorComponent.h"
#include "TimelineCreatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimelineEnded);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTimelineTick, float, val);

UENUM(BlueprintType)
enum class ETimelineType : uint8
{
    Linear,
    Curve,
    Projectile,
};

UCLASS()
class CHOREOPLATFORMER_API UTimelineCreatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimelineCreatorComponent();
    void Initialize();
    void PlayTimeline();
    void Stop(bool bForceFinishAction);
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    bool IsRunning() const;
    bool ReachedHalfwayPoint() const;
    UPROPERTY()
    FTimelineEnded TimelineEnded;

protected:
    UPROPERTY()
    AActor* TimelineTarget;
    UPROPERTY()
    UTimelineComponent* MyTimeline;
    UPROPERTY()
    class UCurveFloat* TimelineCurve;

    UPROPERTY()
    FTimelineTick TimelineTick;

    UFUNCTION()
    void TimelineCallback(float val);
    UFUNCTION()
    void TimelineFinishedCallback();
    virtual void Reset() {}
    virtual void ForceStopTimeline() {}

    UPROPERTY()
    TEnumAsByte<ETimelineDirection::Type> TimelineDirection;
    UPROPERTY()
    float ElapsedTime;
};

UCLASS()
class CHOREOPLATFORMER_API UMovementTimelineComponent : public UTimelineCreatorComponent
{
    GENERATED_BODY()

public:
    UMovementTimelineComponent();
    void Reset() override;
    void ForceStopTimeline() override;

    void MoveToPosition(FVector TargetPosition, float TimelineLength);
    void RotateToPosition(FRotator TargetPosition, float TimelineLength);

protected:

    UPROPERTY()
    FVector OriginLocation;
    UPROPERTY()
    FVector TargetLocation;
    UPROPERTY()
    FRotator OriginRotation;
    UPROPERTY()
    FRotator TargetRotation;

    UFUNCTION()
    void MoveCallback(float val);
    UFUNCTION()
    void RotateCallback(float val);
};

UCLASS()
class CHOREOPLATFORMER_API UColorTimelineComponent : public UTimelineCreatorComponent
{
    GENERATED_BODY()

public:
    UColorTimelineComponent();
    void AddMesh(class UMeshComponent*);
    void Reset() override;
    void ForceStopTimeline() override;

    void Blink();
    void ChangeColor(FColor newColor);
    UFUNCTION()
    void BlinkCallback(float val);

protected:

    UPROPERTY()
    float OriginBrightness;
    UPROPERTY(BlueprintReadWrite)
    float TargetBrightness = 1;
    UPROPERTY()
    class UMaterialInstanceDynamic* ShineMat;
};

UCLASS()
class CHOREOPLATFORMER_API UColorFadeTimelineComponent : public UTimelineCreatorComponent
{
    GENERATED_BODY()

public:
    UColorFadeTimelineComponent();
    void AddMesh(class UMeshComponent*);
    void Reset() override;
    void ForceStopTimeline() override;

    void FadeInDirection(bool Direction);
    UFUNCTION()
    void FadeCallback(float val);

protected:

    UPROPERTY()
    float OriginBrightness;
    UPROPERTY()
    float TargetBrightness = 1;
    UPROPERTY()
    class UMaterialInstanceDynamic* ShineMat;
};

UCLASS()
class CHOREOPLATFORMER_API USpritesTimelineComponent : public UTimelineCreatorComponent
{
    GENERATED_BODY()

public:
    USpritesTimelineComponent();
    void SetSprites(TArray<class UPaperSpriteComponent*>);
    void Reset() override;

    void Blink();
    void ChangeType(const FTileSpriteInfo& spriteType);
    void ChangeBrightness(float Brightness);
    UFUNCTION()
    void OpacityCallback(float val);

protected:
    UPROPERTY()
    float OriginOpacity;
    UPROPERTY(BlueprintReadWrite)
    float TargetOpacity = 1;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<class UTexture2D> SpriteTexture;
    UPROPERTY()
    TArray<class UPaperSpriteComponent*> Sprites;
    UPROPERTY()
    class UMaterialInstanceDynamic* ShineMat;
};

UCLASS()
class CHOREOPLATFORMER_API UProjectileTimelineComponent : public UTimelineCreatorComponent
{
    GENERATED_BODY()

public:
    UProjectileTimelineComponent();
    void Throw(FVector OriginLocation, FVector TargetLocation, float TimelineLength);
    void ForceStopTimeline() override;
    UFUNCTION()
    void LandedCallback(float val);

protected:

    UPROPERTY()
    FVector OriginPosition;
    UPROPERTY()
    FVector TargetPosition;
};

UCLASS()
class CHOREOPLATFORMER_API UScaleUpTimelineComponent : public UTimelineCreatorComponent
{
    GENERATED_BODY()

public:
    UScaleUpTimelineComponent();
    void ScaleUp(FVector OriginScale, FVector TargetScale, float TimelineLength);
    void ForceStopTimeline() override;
    UFUNCTION()
    void ScaleCallback(float val);

    UPROPERTY()
    class USceneComponent* ScalingActor;

protected:

    UPROPERTY()
    FVector OriginScale;
    UPROPERTY()
    FVector TargetScale;
};