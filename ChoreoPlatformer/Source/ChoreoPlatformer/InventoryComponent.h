// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ClothingItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);
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

public:
	UPROPERTY(BlueprintAssignable)
	FInventoryChanged InventoryChanged;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasItem(FGameplayTag ItemType);
	void AddItem(AClothingItem* Item);
	bool RemoveItem(FGameplayTag ItemType);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasHealthItem();
	int HealthItemQuantity();
	bool LoseHealthItem();
};
