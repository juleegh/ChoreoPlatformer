// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface_Highlighter.h"
#include "DanceDefinitions.h"
#include "GameplayTagContainer.h"
#include "ContextualElement.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AContextualElement : public AActor, public IInterface_Highlighter
{
	GENERATED_BODY()

public:
	AContextualElement();

protected:
	UPROPERTY()
	bool bFinished = false;
	virtual void BeginPlay() override;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshState();
	void ToggleHighlight(bool activated) override;
	virtual EMoveResult TriggerInteraction() { return EMoveResult::None; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanInteract() { return !bFinished; }
};

UCLASS()
class CHOREOPLATFORMER_API ABrickWall : public AContextualElement
{
	GENERATED_BODY()

public:
	ABrickWall() {}

protected:
	UPROPERTY()
	int HitsLeft = 2;
	EMoveResult TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API ADoor : public AContextualElement
{
	GENERATED_BODY()

public:
	ADoor() {}
	void Open();

protected:
	EMoveResult TriggerInteraction() override;

};

UCLASS()
class CHOREOPLATFORMER_API ALever : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly)
	TArray<ADoor*> ConnectedDoors;
public:
	ALever() {}
	EMoveResult TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API AItemObstacle : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag RequiredItem;

	void RemoveObstacle();
	virtual void PostObstacleActions() {}
public:
	EMoveResult TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API ATileHole : public AItemObstacle
{
	GENERATED_BODY()

protected:
	void PostObstacleActions() override;
};