#include "ClothingItem.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InventoryComponent.h"

AClothingItem::AClothingItem()
{
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetupAttachment(BoxComponent);
}

void AClothingItem::TriggerInteraction()
{
	if (!bFinished)
	{
		bFinished = true;
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		UDanceUtilsFunctionLibrary::GetInventoryComponent(this)->AddItem(this);
		RefreshState();
	}
}


