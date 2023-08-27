// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TileChallenge.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Components/ActorComponent.h"
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
	TMap<EChallengeType, int> TotalChallenges;
	UPROPERTY()
	TMap<EChallengeType, int> CollectedChallenges;
	UPROPERTY()
	int TotalFruit;

public:
	// Called every frame
	void LoadMap();
	UPROPERTY(EditDefaultsOnly, Category = "Base Tile")
	TSubclassOf<AGridCell> TileBP;
	void CollectChallenge(EChallengeType ChallengeType);
	int GetTotalByChallengeType(EChallengeType ChallengeType);
	int GetCollectedByChallengeType(EChallengeType ChallengeType);
	int GetTotalFruit();
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
	float CurrentSectionStart;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentSection;

public:
	void Initialize();
	UFUNCTION(BlueprintCallable)
	void SectionChanged(FGameplayTag NewSection);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCurrentSection();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayTempoResult(ETempoAccuracy Result);
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
	UPROPERTY()
	TArray<FGameplayTag> Sections;

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
class CHOREOPLATFORMER_API ASectionStart : public AActor
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
