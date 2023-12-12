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

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ATilemapLevelManager : public AActor
{
	GENERATED_BODY()

public:
	ATilemapLevelManager() {}

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	TArray<class AGridCell*> TilePool;
	UPROPERTY()
	TArray<class AGridCell*> WorldTiles;

public:
	void LoadMap(const FGameplayTag& Level);
	UPROPERTY(EditDefaultsOnly, Category = "Base Tile")
	TSubclassOf<AGridCell> TileBP;
	void SpawnTile(FVector Position, ETempoTile TileType, FGameplayTag SectionIdentifier);
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
	class USoundWave* SectionSong;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level")
	float SongBPM;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level")
	FGameplayTag SectionEndTrigger;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentSection;
	UPROPERTY()
	class ASectionStart* CurrentSectionStart;
	bool bIsPlaying;

public:
	void Initialize();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCurrentSection();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayTempoResult(ETempoAccuracy Result);
	const FGameplayTag& GetStartSection() { return StartSection; };
	const FGameplayTag& GetCurrentSection() { return CurrentSection; };
	void CurrentSectionEnd(class ASectionStart* NextSection);
	UFUNCTION(BlueprintCallable)
	void NextSectionStart();
	bool CanMove() { return bIsPlaying; }
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
	TMap<TSubclassOf<UUserWidget>, UUserWidget*> Widgets;
	UPROPERTY()
	TMap<FGameplayTag, int> Countdowns;

	void HandleWidgetEvent(FGameplayTag TriggerTag);
	void HandleCountdownEvent(FGameplayTag TriggerTag);
	void HandleSectionEvent(FGameplayTag TriggerTag);

public:
	void ActivateTrigger(FGameplayTag TriggerTag);
};

UCLASS()
class CHOREOPLATFORMER_API AEventTrigger : public AActor
{
	GENERATED_BODY()

public:
	AEventTrigger();

protected:
	virtual void BeginPlay() override;
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
	ASectionStart() {}

protected:
	UPROPERTY(EditInstanceOnly)
	FGameplayTag SectionIdentifier;

public:
	FGameplayTag GetSectionIdentifier() { return SectionIdentifier; }
};
