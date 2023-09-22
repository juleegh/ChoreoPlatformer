// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagsManager.h"
#include "Blueprint/UserWidget.h"
#include "EventsDataAsset.generated.h"

USTRUCT(BlueprintType)
struct CHOREOPLATFORMER_API FWidgetLevelEvent
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		bool bSpawnsWidget;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UUserWidget> LevelWidget;
};

UCLASS()
class CHOREOPLATFORMER_API UEventsDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, FWidgetLevelEvent> WidgetEvents;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, int> CountdownEvents;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FGameplayTag> Sections;
};
