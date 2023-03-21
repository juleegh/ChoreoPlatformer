// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "TutorialLevelActor.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AChoreoLevelActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Setup")
	class USoundBase* Song;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Setup")
	float SongFrequency;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Setup")
	int IntroTempos;
};

UCLASS()
class CHOREOPLATFORMER_API ATutorialLevelActor : public AChoreoLevelActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void CallibrationEnded();
};

