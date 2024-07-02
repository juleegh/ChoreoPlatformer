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
	void Init() override;
	FGameplayTag CurrentLevel;

	UFUNCTION()
	void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	void EndLoadingScreen(UWorld* InLoadedWorld);
};
