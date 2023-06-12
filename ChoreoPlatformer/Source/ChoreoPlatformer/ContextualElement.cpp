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
	if (ConnectedDoor)
	{
		RefreshState();
		ConnectedDoor->BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ConnectedDoor->RefreshState();
		ConnectedDoor = nullptr;
	}
}

void AItem::TriggerInteraction()
{
	if (!bFinished)
	{
		bFinished = true;
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		UDanceUtilsFunctionLibrary::GetInventoryComponent(GetWorld())->AddItem(ItemType);
		RefreshState();
	}
}

void AItemObstacle::TriggerInteraction()
{
	if (UDanceUtilsFunctionLibrary::GetInventoryComponent(GetWorld())->HasItem(RequiredItem) && !bFinished)
	{
		bFinished = true;
		RemoveObstacle();
	}
}

void AItemObstacle::RemoveObstacle()
{
	UDanceUtilsFunctionLibrary::GetInventoryComponent(GetWorld())->RemoveItem(RequiredItem);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PostObstacleActions();
	RefreshState();
}

void ATileHole::PostObstacleActions()
{
	UDanceUtilsFunctionLibrary::GetTilemapLevelManager(GetWorld())->SpawnTile(GetActorLocation(), ETempoTile::Black, FGameplayTag::EmptyTag);
}

