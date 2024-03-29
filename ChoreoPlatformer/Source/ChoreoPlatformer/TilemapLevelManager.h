// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Components/ActorComponent.h"
#include "GridElement.h"
#include "EventsDataAsset.h"
#include "TilemapLevelManager.generated.h"

class UPaperTileMap;
class AGridCell;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelEnd);

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ATilemapLevelManager : public AActor
{
	GENERATED_BODY()

public:
	ATilemapLevelManager() {}

protected:
	UPROPERTY()
	TArray<AGridCell*> TilePool;
	UPROPERTY()
	TArray<AGridCell*> WorldTiles;

public:
	void LoadMap(const FGameplayTag& Level);
	void LoadTileMap(const UPaperTileMap* TileMap, FVector AnchorLocation, FGameplayTag SectionIdentifier);
	UPROPERTY(EditDefaultsOnly, Category = "Base Tile")
	TSubclassOf<AGridCell> TileBP;
	void SpawnTile(FVector Position, FRotator DeltaRotation, ETempoTile TileType, FGameplayTag SectionIdentifier);
	void ClearTile(AGridCell* ClearingTile);
	TArray<AGridCell*>* GetWorldTiles();
};

UCLASS()
class CHOREOPLATFORMER_API ASectionLevelManager : public AActor
{
	GENERATED_BODY()

public:
	ASectionLevelManager() {}

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level")
	FGameplayTag StartSection;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level")
	float SongBPM;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentSection;
	UPROPERTY()
	class ASectionStart* CurrentSectionStart;
	UPROPERTY(BlueprintAssignable)
	FLevelStart LevelStart;
	UPROPERTY(BlueprintAssignable)
	FLevelEnd LevelEnd;
	UPROPERTY()
	FTimerHandle DelayTimerHandle;

public:
	void BeginPlay() override;
	const FGameplayTag& GetStartSection() { return StartSection; };
	const FGameplayTag& GetCurrentSection() { return CurrentSection; };
	void CurrentSectionEnd(class ASectionStart* NextSection);
	void CurrentSectionEnd(FGameplayTag NextSection);
	UFUNCTION(BlueprintCallable)
	void NextSectionStart();
	void StartFromSection(const FGameplayTag SectionIdentifier);
};


USTRUCT(BlueprintType)
struct CHOREOPLATFORMER_API FProceduralTileInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETempoTile TileType;
	UPROPERTY(BlueprintReadOnly)
	FVector ForcedDirection;
};

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API AEndlessLevelManager : public AActor
{
	GENERATED_BODY()

public:
	AEndlessLevelManager() {}

protected:

	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	TArray<TObjectPtr<UPaperTileMap>> AvailableTileMaps;

	UPROPERTY(EditInstanceOnly, Category = "Progression")
	int InitialSongBPM = 110;
	
	UPROPERTY()
	TMap<FVector, FProceduralTileInfo> CurrentTiles;
	UPROPERTY()
	TMap<FVector, FProceduralTileInfo> NextTiles;
	UPROPERTY()
	float TileDimension;
	UPROPERTY()
	int TileMapDimension;
	UPROPERTY()
	int CurrentLine;

	UFUNCTION(BlueprintCallable)
	void Initialize();
	UFUNCTION(BlueprintCallable)
	void ShuffleWorldDown();
	void LoadNextMap(UPaperTileMap* TileMap);

};

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ULevelEventsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelEventsComponent();

protected:

	UPROPERTY()
	UEventsDataAsset* LevelEvents;
	UPROPERTY()
	TMap<FGameplayTag, int> Countdowns;
	UPROPERTY()
	FLevelEventInfo LastEventData;

	void HandleCountdownEvent(FGameplayTag TriggerTag);
	void HandleSectionEvent(FGameplayTag TriggerTag);
	void ActivateTrigger(FGameplayTag TriggerTag);

public:
	void HandleEvent(FLevelEventInfo LevelEventInfo);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLevelEventInfo GetLastEventData() { return LastEventData; }
};

UCLASS()
class CHOREOPLATFORMER_API AEventTrigger : public AActor
{
	GENERATED_BODY()

public:
	AEventTrigger();
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetFlavorTrigger() { return FlavorTrigger; }

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UBoxComponent* BoxComponent;
	UPROPERTY(EditInstanceOnly)
	FGameplayTag ActorTrigger;
	UPROPERTY(EditInstanceOnly)
	FGameplayTag FlavorTrigger;
	UFUNCTION()
	void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

UCLASS()
class CHOREOPLATFORMER_API ASectionStart : public ACheckpoint
{
	GENERATED_BODY()

public:
	ASectionStart() {}

protected:
	UPROPERTY(EditInstanceOnly)
	FGameplayTag SectionIdentifier;

public:
	FGameplayTag GetSectionIdentifier() { return SectionIdentifier; }
};
