// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "ChoreoLevelActor.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AChoreoLevelActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AChoreoLevelActor() {}

protected:
	virtual void BeginPlay() override;
};

UCLASS()
class CHOREOPLATFORMER_API ATutorialLevelActor : public AChoreoLevelActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void CallibrationEnded();
};

