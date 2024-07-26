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
	void BeginPlay() override;
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
	UPROPERTY()
	class UMaterialInstanceDynamic* ObjectMat;
	UPROPERTY(BlueprintReadOnly)
	bool bFinished = false;
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshState();
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	UFUNCTION()
	void LandedOnGround();
public:
	void OnEnterRange() override;
	FGameplayTag& GetItemType() { return ItemType; }
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleHighlight(bool activated);
	void PutBack(FVector NewPosition = FVector(0,0,0), bool bToOriginalPosition = false);
};