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
	ATilemapLevelManager()
	{
		PrimaryActorTick.bCanEverTick = false;
	}

protected:
	UPROPERTY()
	TArray<AGridCell*> TilePool;
	UPROPERTY()
	TArray<AGridCell*> WorldTiles;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<class UPaperTileSet> TileSet;

public:
	void LoadMap(const FGameplayTag& Level);
	void LoadTileMap(const UPaperTileMap* TileMap, const FVector& AnchorLocation, const FGameplayTag& SectionIdentifier);
	UPROPERTY(EditDefaultsOnly, Category = "Base Tile")
	TSubclassOf<AGridCell> TileBP;
	void SpawnTile(const FVector& Position, const FRotator& DeltaRotation, const ETempoTile TileType, const FGameplayTag& SectionIdentifier);
	void ClearTile(AGridCell* ClearingTile);
	TArray<AGridCell*>* GetWorldTiles();
};

UCLASS()
class CHOREOPLATFORMER_API ASectionLevelManager : public AActor
{
	GENERATED_BODY()

public:
	ASectionLevelManager()
	{
		PrimaryActorTick.bCanEverTick = false;
	}

	UPROPERTY(BlueprintAssignable)
	FLevelStart LevelStart;
	UPROPERTY(BlueprintAssignable)
	FLevelEnd LevelEnd;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level")
	FGameplayTag StartSection;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level")
	float SongBPM;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentSection;
	UPROPERTY()
	class ASectionStart* CurrentSectionStart;
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
	AEndlessLevelManager()
	{
		PrimaryActorTick.bCanEverTick = false;
	}
	void PlayerMoved(FVector NewPosition);
	bool ShouldShuffleWorldInstead(FVector NextPosition);
	void ShuffleWorldDown(float ElapsedTime);

protected:

	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	TArray<TObjectPtr<UPaperTileMap>> AvailableTileMaps;
	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	TArray<TObjectPtr<UStaticMesh>> AvailableCityMeshes;
	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	TSubclassOf<class ACityMesh> BaseMesh;
	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	FColor Color1;
	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	FColor Color2;
	UPROPERTY(EditInstanceOnly, Category = "Tile Generation")
	FColor Color3;
	
	UPROPERTY()
	TArray<TObjectPtr<ACityMesh>> MeshPool;
	UPROPERTY()
	TArray<TObjectPtr<ACityMesh>> WorldMeshes;

	UPROPERTY(EditInstanceOnly, Category = "Progression")
	float InitialSongBPM = 110;
	
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
	void LoadNextMap(UPaperTileMap* TileMap);
	void SpawnCityMesh(FVector Position);

};

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ULevelEventsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelEventsComponent();

protected:

	UPROPERTY()
	FLevelEventInfo LastEventData;

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

protected:
	void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditInstanceOnly)
	FGameplayTag ActorTrigger;
	UFUNCTION()
	void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

UCLASS()
class CHOREOPLATFORMER_API ASectionStart : public ACheckpoint
{
	GENERATED_BODY()

public:
	ASectionStart()
	{
		PrimaryActorTick.bCanEverTick = false;
	}

protected:
	UPROPERTY(EditInstanceOnly)
	FGameplayTag SectionIdentifier;

public:
	FGameplayTag GetSectionIdentifier() { return SectionIdentifier; }
};
