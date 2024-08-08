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
	float GetTempoDuration();

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

UCLASS()
class CHOREOPLATFORMER_API AWater : public AContextualElement
{
	GENERATED_BODY()

public:
	AWater();
	void Reset() override;
	bool ChangeWaterLevel(float Direction);
	void ToggleHighlight(bool activated) override;
	UPROPERTY(EditInstanceOnly)
	TArray<class ACityMesh*> FloatingActors;
	UPROPERTY(EditInstanceOnly)
	TArray<class AWaterButton*> ConnectedButtons;
	UPROPERTY(EditInstanceOnly)
	TArray<class AWaterTileAnchor*> FloatingTiles;

protected:
	void BeginPlay() override;
	EMoveResult TriggerInteraction() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float ScaleFactor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* WaterMesh;
	UPROPERTY(EditInstanceOnly, Category = "Water Editing")
	float Height;
	UPROPERTY(EditInstanceOnly, Category = "Water Editing")
	float WidthX;
	UPROPERTY(EditInstanceOnly, Category = "Water Editing")
	float WidthY;
	UPROPERTY(EditInstanceOnly, Category = "Water Editing")
	float MinLevel;
	UPROPERTY(EditInstanceOnly, Category = "Water Editing")
	float MaxLevel;
	UPROPERTY()
	float InitialLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UMovementTimelineComponent* MoveTimeline;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

UCLASS()
class CHOREOPLATFORMER_API AWaterButton : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly)
	TSoftObjectPtr<AWater> ConnectedWater;
	UPROPERTY(EditDefaultsOnly)
	float Direction = 1;
public:
	AWaterButton() {}
	EMoveResult TriggerInteraction() override;
	void ToggleHighlight(bool activated) override;
};

UCLASS()
class CHOREOPLATFORMER_API AWaterTileAnchor : public AContextualElement
{
	GENERATED_BODY()

public:
	AWaterTileAnchor();
	void Reset() override;
	void ToggleHighlight(bool activated) override;
	void Float(FVector Delta);

protected:
	void BeginPlay() override;
	UPROPERTY()
	float InitialLevel; 
	UPROPERTY()
	class AGridCell* Tile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UMovementTimelineComponent* MoveTimeline;
};

UCLASS()
class CHOREOPLATFORMER_API ADynamicTileAnchor : public AContextualElement
{
	GENERATED_BODY()

public:
	void Reset() override;
	void ToggleHighlight(bool activated) override;
	void Repaint(ETempoTile TileType);
	class ATileChangerButton* BelongingLever;

protected:
	UPROPERTY()
	class AGridCell* Tile;
};

UCLASS()
class CHOREOPLATFORMER_API ATileChangerButton : public AContextualElement
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	UPROPERTY(EditInstanceOnly)
	TArray<ADynamicTileAnchor*> ConnectedTiles;
	UPROPERTY(EditInstanceOnly)
	TArray<ETempoTile> TypesOfTile;
	UPROPERTY()
	int TileIndex = 0;
public:
	EMoveResult TriggerInteraction() override;
	void ToggleHighlight(bool activated) override;
};