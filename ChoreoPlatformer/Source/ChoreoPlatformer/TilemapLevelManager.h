// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TileChallenge.h"
#include "DanceUtilsFunctionLibrary.h"
#include "TilemapLevelManager.generated.h"

class UPaperTileMap;
class AGridCell;


USTRUCT(BlueprintType)
struct FSongSectionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FGameplayTag SectionName;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class USoundWave* Song;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float SongBPM;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float BeatStart;
};

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ATilemapLevelManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ATilemapLevelManager() {}

protected:
	// Called when the game starts
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
	TMap<FGameplayTag, FSongSectionInfo> Sections;
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentSection;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSectionStart;
	UPROPERTY(BlueprintReadOnly)
	class USoundWave* SectionSong;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float CurrentSongBPM;
	
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void SectionChanged(FGameplayTag NewSection);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCurrentSection();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayTempoResult(ETempoAccuracy Result);
};
