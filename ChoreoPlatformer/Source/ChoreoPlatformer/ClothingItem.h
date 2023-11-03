// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface_Highlighter.h"
#include "GameplayTagContainer.h"
#include "GridElement.h"
#include "ClothingItem.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AClothingItem : public AGridElement, public IInterface_Highlighter
{
	GENERATED_BODY()

public:
	AClothingItem();
protected:
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	FGameplayTag ItemType;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UStaticMeshComponent* ItemMesh;
	UPROPERTY()
	bool bFinished = false;
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshState();
public:
	void OnEnterRange() override;
	FGameplayTag GetItemType() { return ItemType; }
	void ToggleHighlight(bool activated) override;
};