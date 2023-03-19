// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridCell.h"
#include "TileDetectorComponent.generated.h"

USTRUCT()
struct FDetectedInfo
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitElement;
	UPROPERTY()
	class AContextualElement* HitElement;
	UPROPERTY()
	ETempoTile TileType;
	UPROPERTY()
	float TargetTempo = 1;
	UPROPERTY()
	bool bForcesDirection;
	UPROPERTY()
	FVector ForcedDirection;
};

UCLASS(ClassGroup = (Custom), meta = (IsBlueprintBase = "true"))
class CHOREOPLATFORMER_API UTileDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTileDetectorComponent();

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	AActor* PlayerPawn;

public:
	FDetectedInfo CheckPosition(FVector Start);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
