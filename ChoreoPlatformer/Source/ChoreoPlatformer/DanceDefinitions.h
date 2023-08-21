#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "DanceDefinitions.generated.h"

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

UENUM(BlueprintType)
enum class ETempoAccuracy : uint8
{
	Bad,
	Great,
	Perfect,
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	OneTempoBlock,
	Fruit,
};

USTRUCT(BlueprintType)
struct FTileInfo
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly)
		bool bHitElement = false;
	UPROPERTY(BlueprintReadOnly)
		class AContextualElement* HitElement;
	UPROPERTY(BlueprintReadOnly)
		ETempoTile TileType;
	UPROPERTY(BlueprintReadOnly)
		FVector Position;
	UPROPERTY(BlueprintReadOnly)
		float TargetTempo = 1;
	UPROPERTY(BlueprintReadOnly)
		bool bForcesDirection;
	UPROPERTY(BlueprintReadOnly)
		FVector ForcedDirection;
	UPROPERTY(BlueprintReadOnly)
		FGameplayTag Section;
};

USTRUCT(BlueprintType)
struct CHOREOPLATFORMER_API FClothingItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Identifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BodySection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;
};