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
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Meta = (Category = "ClothingItem"))
	FGameplayTag ItemType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (Category = "ClothingItem"))
	FGameplayTag BodySocket;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UDataTable* ItemsData;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (Category = "ClothingItem"))
	class UStaticMeshComponent* ItemMesh;
	UPROPERTY()
	class UProjectileTimelineComponent* ProjectileTimeline;
	UPROPERTY()
	FVector OriginPosition;
	UPROPERTY()
	FVector OriginScale;
	UPROPERTY(BlueprintReadOnly)
	bool bFinished = false;
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshState();
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	UFUNCTION()
	void LandedOnGround();
public:
	void OnEnterRange() override;
	FGameplayTag GetItemType() { return ItemType; }
	void ToggleHighlight(bool activated) override;
	void PutBack(FVector NewPosition = FVector(0,0,0), bool bToOriginalPosition = false);
};