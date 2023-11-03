#include "ContextualElement.h"
#include "Components/BoxComponent.h"
#include "LevelProgressComponent.h"
#include "ChoreoPlayerController.h"
#include "DancerHealthComponent.h"
#include "InventoryComponent.h"
#include "TilemapLevelManager.h"
#include "DanceCharacter.h"

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

void ABrickWall::TriggerInteraction()
{
	HitsLeft--;
	if (HitsLeft <= 0)
	{
		RefreshState();
	}
}

void ALever::TriggerInteraction()
{
	if (ConnectedDoors.Num() > 0)
	{
		RefreshState();
		for (ADoor* ConnectedDoor : ConnectedDoors)
		{
			ConnectedDoor->BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			ConnectedDoor->RefreshState();
			auto DoorTile = UDanceUtilsFunctionLibrary::CheckPosition({ ConnectedDoor }, ConnectedDoor->GetActorLocation());
			if (DoorTile.HitCell)
			{
				DoorTile.HitCell->PromptTrigger();
			}
		}
		ConnectedDoors.Empty();
	}
}

void AItemObstacle::TriggerInteraction()
{
	if (UDanceUtilsFunctionLibrary::GetInventoryComponent(this)->HasItem(RequiredItem) && !bFinished)
	{
		bFinished = true;
		RemoveObstacle();
	}
}

void AItemObstacle::RemoveObstacle()
{
	UDanceUtilsFunctionLibrary::GetInventoryComponent(this)->RemoveItem(RequiredItem);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ToggleHighlight(false);
	PostObstacleActions();
	RefreshState();
}

void ATileHole::PostObstacleActions()
{
	UDanceUtilsFunctionLibrary::GetTilemapLevelManager(GetWorld())->SpawnTile(GetActorLocation(), ETempoTile::Black, FGameplayTag::EmptyTag);
}

