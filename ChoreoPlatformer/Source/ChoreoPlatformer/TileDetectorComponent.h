// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TileDetectorComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (IsBlueprintBase = "true"))
class CHOREOPLATFORMER_API UTileDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTileDetectorComponent();

public:
	void CheckTile();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
