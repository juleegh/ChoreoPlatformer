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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* HitBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* Base;
	UPROPERTY(BlueprintReadOnly)
	class UColorTimelineComponent* ColorTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	FColor DamageColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Feedback")
	FColor TriggerColor;

public:	
	// Called every frame
	void Initialize(ETempoTile, FGameplayTag&);
	UFUNCTION(BlueprintImplementableEvent)
	void PaintTile();
	virtual void Tick(float DeltaTime) override;
	ETempoTile GetTileType() { return TileType; }
	bool ForcesPlayerPosition();
	FVector ForcedDirection();
	FGameplayTag& GetSection();
	void PromptTrigger();
	void PromptDamage();
};
