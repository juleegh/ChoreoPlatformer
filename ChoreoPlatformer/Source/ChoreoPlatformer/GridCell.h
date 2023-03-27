// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GridCell.generated.h"

UENUM(BlueprintType)
enum class ETempoTile : uint8
{
	Black,
	Half,
	Blocker,
	ForceUp,
	ForceDown,
	ForceLeft,
	ForceRight,
};

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
	UPROPERTY(BlueprintReadOnly)
	class USongTempoComponent* SongTempo;

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
};
