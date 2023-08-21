// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ContextualElement.h"
#include "ClothingItem.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AClothingItem : public AContextualElement
{
	GENERATED_BODY()

public:
	AClothingItem();
protected:
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	FGameplayTag ItemType;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UStaticMeshComponent* ItemMesh;
	UPROPERTY()
	bool bFinished;
public:
	void TriggerInteraction() override;
	FGameplayTag GetItemType() { return ItemType; }
};