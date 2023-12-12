// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "ChoreoGameInstance.generated.h"

/**
 *
 */
UCLASS()
class CHOREOPLATFORMER_API UChoreoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	FGameplayTag CurrentLevel;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);
};
