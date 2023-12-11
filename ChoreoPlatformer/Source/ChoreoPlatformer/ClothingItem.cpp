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

void AClothingItem::BeginPlay()
{
	Super::BeginPlay();
	ToggleHighlight(false);
}

void AClothingItem::OnEnterRange()
{
	if (!bFinished)
	{
		bFinished = true;
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		UDanceUtilsFunctionLibrary::GetInventoryComponent(this)->AddItem(this);
		RefreshState();
	}
}

void AClothingItem::ToggleHighlight(bool activated)
{
	ItemMesh->SetRenderCustomDepth(activated);
}

void AClothingItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	TArray<FClothingItemInfo*> Items;
	ItemsData->GetAllRows<FClothingItemInfo>(TEXT("ContextString"), Items);
	for (auto ItemInfo : Items)
	{
		if (ItemInfo->Identifier.MatchesTag(ItemType) && ItemInfo->Mesh)
		{
			ItemMesh->SetStaticMesh(ItemInfo->Mesh);
			BodySocket = ItemInfo->BodySocket;
			return;
		}
	}
}



