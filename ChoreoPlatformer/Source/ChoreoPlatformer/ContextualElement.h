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
	void BeginPlay() override;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* InteractionHighlight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UColorFadeTimelineComponent* ColorTimeline;
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshState();
	void ToggleHighlight(bool activated) override;
	virtual EMoveResult TriggerInteraction() { return EMoveResult::None; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanInteract() { return !bFinished; }
	virtual void Reset();
	virtual bool BlocksPlayerMovement() { return true; }
	UFUNCTION(BlueprintImplementableEvent)
	void OnReset();
};

UCLASS()
class CHOREOPLATFORMER_API ABrickWall : public AContextualElement
{
	GENERATED_BODY()

public:
	ABrickWall() {}
	EMoveResult TriggerInteraction() override;

protected:
	UPROPERTY()
	int HitsLeft = 2;
};

UCLASS()
class CHOREOPLATFORMER_API ADoor : public AContextualElement
{
	GENERATED_BODY()

public:
	ADoor() {}
	void Open();
	void Reset() override;
	UPROPERTY()
	class ALever* BelongingLever;
	void ToggleHighlight(bool activated) override;
	EMoveResult TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API ALever : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly)
	TArray<ADoor*> ConnectedDoors;
	void BeginPlay() override;
public:
	ALever() {}
	EMoveResult TriggerInteraction() override;
	void ToggleHighlight(bool activated) override;
};

UCLASS()
class CHOREOPLATFORMER_API ARotatingAnchor : public AContextualElement
{
	GENERATED_BODY()

public:
	ARotatingAnchor() {}
	void Reset() override;
	void Rotate(float Direction);
	UPROPERTY()
	class ARotationButton* BelongingLever;
	void ToggleHighlight(bool activated) override;

protected:
	UPROPERTY()
	FRotator InitialRotation;
	UPROPERTY()
	class AGridCell* Tile;
};

UCLASS()
class CHOREOPLATFORMER_API ARotationButton : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly)
	TArray<ARotatingAnchor*> ConnectedTiles;
	UPROPERTY(EditDefaultsOnly)
	float Direction = 90;
	void BeginPlay() override;
public:
	ARotationButton() {}
	EMoveResult TriggerInteraction() override;
	void ToggleHighlight(bool activated) override;
};

UCLASS()
class CHOREOPLATFORMER_API ATileHole : public AContextualElement
{
	GENERATED_BODY()

public:
	void Reset() override;
	EMoveResult TriggerInteraction() override;
	void ToggleHighlight(bool activated) override;
	void CoverHole();
};

UCLASS()
class CHOREOPLATFORMER_API APlacingTile : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FVector InitialPosition;
	void BeginPlay() override;

public:
	EMoveResult TriggerInteraction() override;
	void ToggleHighlight(bool activated) override;
	void Reset() override;
	bool BlocksPlayerMovement() override { return false; }
};