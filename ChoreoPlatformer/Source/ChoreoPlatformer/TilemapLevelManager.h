// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
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
	float BeatStart;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float BeatDuration;
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

public:
	// Called every frame
	void LoadMap();
	UPROPERTY(EditDefaultsOnly, Category = "Base Tile")
	TSubclassOf<AGridCell> TileBP;
};

UCLASS()
class CHOREOPLATFORMER_API ASectionLevelManager : public AActor
{
	GENERATED_BODY()

public:
	ASectionLevelManager() {}

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level Song")
	class USoundWave* Song;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level Song")
	float SongBPM;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level Song")
	int IntroTempos;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level Song")
	TMap<FGameplayTag, FSongSectionInfo> Sections;
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentSection;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSectionStart;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSectionDuration;
	
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void SectionChanged(FGameplayTag NewSection);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCurrentSection();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayTempoResult(ETempoAccuracy Result);
};
