// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelProgressComponent.generated.h"

class ACheckpoint;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHOREOPLATFORMER_API ULevelProgressComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelProgressComponent();

protected:
	UPROPERTY()
	ACheckpoint* CurrentCheckpoint;

public:	
	void ToggleCheckpoint(ACheckpoint* NewCheckpoint);
	void LoadCheckpoint();
};
