// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "DanceDefinitions.h"
#include "Engine/DataTable.h"
#include "PreviewPigeon.generated.h"

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API APreviewPigeon : public ACharacter
{
	GENERATED_BODY()

public:
	APreviewPigeon();
	void LoadItemData();
	TMap<FGameplayTag, bool>& GetItems();
	void PutOnItem(FGameplayTag ItemType);
	void Clear();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<class AActor*>& GetRenderingActors() { return RenderingActors; }

protected:
	UPROPERTY()
	TArray<class AActor*> RenderingActors;
	UPROPERTY()
	TMap<FGameplayTag, class AActor*> CurrentOutfit;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* ItemsData;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ClothesBP;
	UPROPERTY()
	TMap<FGameplayTag, bool> OwnedItems;

	bool HasCollectedItem(FGameplayTag ItemType);
	FClothingItemInfo& GetClothingItem(FGameplayTag ItemType);
};