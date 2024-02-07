// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ClothingItem.h"
#include "DanceDefinitions.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryChanged, bool, bPositiveDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryCleared);
UCLASS()
class CHOREOPLATFORMER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	UPROPERTY()
	TArray<AClothingItem*> Outfit;
	UPROPERTY()
	TArray<FGameplayTag> Inventory;
	UPROPERTY()
	class UDataTable* ItemsData;
	UPROPERTY()
	TMap<FGameplayTag, FClothingItemInfo> ClothingInfo;

	void LoadCollectables();

public:
	UPROPERTY(BlueprintAssignable)
	FInventoryChanged InventoryChanged;
	UPROPERTY(BlueprintAssignable)
	FInventoryCleared InventoryCleared;
	
	void AddItem(AClothingItem* Item);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasHealthItem();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int HealthItemQuantity();
	bool LoseHealthItem();
	void ClearItemsEndOfLevel();
	FName GetBodySection(FGameplayTag ItemType);
};
