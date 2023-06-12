// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UCLASS()
class CHOREOPLATFORMER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent() {}

protected:
	TArray<FGameplayTag> Inventory;

public:
	bool HasItem(FGameplayTag ItemType);
	void AddItem(FGameplayTag ItemType);
	bool RemoveItem(FGameplayTag ItemType);
};
