// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DanceDefinitions.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GridCell.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AGridCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag Section;
	UPROPERTY(BlueprintReadOnly)
	ETempoTile TileType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UPaperFlipbookComponent* Flipbook;
	UPROPERTY(BlueprintReadOnly)
	class USongTempoComponent* SongTempo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<class UPaperSpriteComponent*> HitSprites;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* Base;
	UPROPERTY(BlueprintReadOnly)
	class USpritesTimelineComponent* SpritesTimeline;
	UPROPERTY()
	class UMovementTimelineComponent* MoveTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	TMap<FGameplayTag, FTileSpriteInfo> SpritesInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	FGameplayTag TriggerType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	FGameplayTag ItemType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	FGameplayTag ExitType;

public:	
	// Called every frame
	void Initialize(ETempoTile, FGameplayTag&);
	UFUNCTION(BlueprintImplementableEvent)
	void PaintTile();
	UFUNCTION(BlueprintImplementableEvent)
	void GetSprites();
	ETempoTile GetTileType() { return TileType; }
	bool ForcesPlayerPosition();
	FVector ForcedDirection();
	FGameplayTag& GetSection();
	void PromptItem();
	void PromptTrigger();
	void PromptDamage();
	void RotateToDirection(FRotator Rotation);
	UFUNCTION(BlueprintCallable)
	void ToggleStaticTrigger(const FGameplayTag& SpriteType, bool bVisible);
};
