#include "ContextualElement.h"
#include "Components/BoxComponent.h"
#include "ChoreoPlayerController.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"

AContextualElement::AContextualElement()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxComponent;
}

void AContextualElement::BeginPlay()
{
	Super::BeginPlay();
	ToggleHighlight(false);
}

void AContextualElement::ToggleHighlight(bool activated)
{
	for (UActorComponent* ActorComponent : GetComponents())
	{
		if(UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(ActorComponent))
		{
			MeshComponent->SetRenderCustomDepth(activated);
		}
	}
}

void AContextualElement::Reset()
{
	bFinished = false;
	OnReset();
}

EMoveResult ABrickWall::TriggerInteraction()
{
	HitsLeft--;
	if (HitsLeft <= 0)
	{
		bFinished = true;
		RefreshState();
	}
	return EMoveResult::None;
}

void ADoor::Open()
{
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	bFinished = true;
	RefreshState();
}

void ADoor::Reset()
{
	Super::Reset();
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

EMoveResult ADoor::TriggerInteraction()
{
	return EMoveResult::Blocked;
}

EMoveResult ALever::TriggerInteraction()
{
	if (ConnectedDoors.Num() > 0)
	{
		RefreshState();
		for (ADoor* ConnectedDoor : ConnectedDoors)
		{
			ConnectedDoor->Open();
			auto DoorTile = UDanceUtilsFunctionLibrary::CheckPosition({ ConnectedDoor }, ConnectedDoor->GetActorLocation());
			if (DoorTile.HitCell)
			{
				DoorTile.HitCell->PromptTrigger();
			}
		}
		bFinished = true;
		return EMoveResult::ActionCompleted;
	}
	return EMoveResult::None;
}

EMoveResult AItemObstacle::TriggerInteraction()
{
	//if (ComponentGetters::GetInventoryComponent(GetWorld)->HasItem(RequiredItem) && !bFinished)
	{
		bFinished = true;
		RemoveObstacle();
		return EMoveResult::None;
	}
}

void AItemObstacle::RemoveObstacle()
{
	//ComponentGetters::GetInventoryComponent(GetWorld())->RemoveItem(RequiredItem);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ToggleHighlight(false);
	PostObstacleActions();
	RefreshState();
}

void ATileHole::PostObstacleActions()
{
	ComponentGetters::GetTilemapLevelManager(GetWorld())->SpawnTile(GetActorLocation(), GetActorRotation(), ETempoTile::Black, FGameplayTag::EmptyTag);
}

void ARotatingAnchor::Reset()
{
	if (Tile)
	{
		Tile->SetActorRotation(InitialRotation);
		Tile = nullptr;
	}
}

void ARotatingAnchor::Rotate(float Direction)
{
	if (!Tile)
	{
		auto TileInfo = UDanceUtilsFunctionLibrary::CheckPosition({this, ComponentGetters::GetDanceCharacter(GetWorld())}, GetActorLocation());
		Tile = TileInfo.HitCell;
		InitialRotation = Tile->GetActorRotation();
	}
	Tile->RotateToDirection(FRotator(0, Direction, 0));
}

EMoveResult ARotationButton::TriggerInteraction()
{
	if (ConnectedTiles.Num() > 0)
	{
		RefreshState();
		for (ARotatingAnchor* ConnectedTile : ConnectedTiles)
		{
			ConnectedTile->Rotate(Direction);
		}
		return EMoveResult::ActionCompleted;
	}
	return EMoveResult::None;
}

