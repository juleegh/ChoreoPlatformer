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
	Ending,
	ForceDirection,
	ForcedChangeable,
	Empty,
	Floater,
	MAX,
};

UENUM(BlueprintType)
enum class ETempoAccuracy : uint8
{
	Bad,
	Great,
	Perfect,
};

UENUM(BlueprintType)
enum class EMoveResult : uint8
{
	Black_OK,
	Half_OK,
	Bad,
	InvalidDirection,
	Blocked,
	ActionCompleted,
	Calibrating,
	ItemGained,
	PlayerHit,
	PlayerDied,
	EnemyOK,
	ObjectMoved,
	None,
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
	bool bBlockedByElement = false;
	UPROPERTY(BlueprintReadOnly)
	bool bHitEnemy = false;
	UPROPERTY(BlueprintReadOnly)
	bool bHitPlayer = false;
	UPROPERTY(BlueprintReadOnly)
	bool bHitWater = false;
	UPROPERTY(BlueprintReadOnly)
	class AContextualElement* HitElement = nullptr;
	UPROPERTY(BlueprintReadOnly)
	class ACheckpoint* HitCheckpoint = nullptr;
	UPROPERTY(BlueprintReadOnly)
	class AGridCell* HitCell = nullptr;
	UPROPERTY(BlueprintReadOnly)
	class APaperTileMapActor* TileMapActor = nullptr;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "ClothingItem"))
	FGameplayTag Identifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "BodySocket"))
	FGameplayTag BodySocket;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Icon"))
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Model"))
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Model"))
	UMaterial* Material;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Model"))
	FColor Color1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Model"))
	FColor Color2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Model"))
	FColor Color3;
};

USTRUCT(BlueprintType)
struct CHOREOPLATFORMER_API FTileSpriteInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Icon"))
	class UPaperSprite* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "Color"))
	FColor Color;
};

USTRUCT(BlueprintType)
struct FPigeonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FRotator PigeonRotation;
	UPROPERTY(EditAnywhere)
	bool bForcePigeonLocation;
	UPROPERTY(EditAnywhere)
	FVector PigeonWorldLocation;
};

USTRUCT(BlueprintType)
struct CHOREOPLATFORMER_API FLevelEventInfo
{
	GENERATED_BODY()

	FLevelEventInfo() {}
	FLevelEventInfo(FGameplayTag Trigger) : EventTrigger(Trigger) {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Category = "EventInfo"))
	FGameplayTag EventTrigger;
};