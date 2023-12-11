#include "InventoryComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "DanceCharacter.h"
#include "DancerHealthComponent.h"
#include "Components/SkeletalMeshComponent.h"

UInventoryComponent::UInventoryComponent()
{
	ItemsData = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/TileElements/ItemDataTable.ItemDataTable'")));
}

void UInventoryComponent::LoadCollectables()
{
	TArray<FClothingItemInfo*> Items;
	ItemsData->GetAllRows<FClothingItemInfo>(TEXT("ContextString"), Items);
	for (auto ItemInfo : Items)
	{
		ClothingInfo.Add(ItemInfo->Identifier, *ItemInfo);
	}
}

void UInventoryComponent::AddItem(AClothingItem* Item)
{
	Outfit.Add(Item);
	InventoryChanged.Broadcast();
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(UDanceUtilsFunctionLibrary::GetDanceCharacter(GetOwner())->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	TransformRules.ScaleRule = EAttachmentRule::KeepWorld;
	Item->AttachToComponent(SkeletalMesh, TransformRules, GetBodySection(Item->GetItemType()));
}

bool UInventoryComponent::HasHealthItem()
{
	return !Outfit.IsEmpty();
}

int UInventoryComponent::HealthItemQuantity()
{
	return Outfit.Num();
}

bool UInventoryComponent::LoseHealthItem()
{
	if (!HasHealthItem())
	{
		return false;
	}
	auto Last = Outfit.Last();
	Outfit.Remove(Last);
	Last->Destroy();
	return true;
}

void UInventoryComponent::ClearItemsEndOfLevel()
{
	for (auto ClothingItem : Outfit)
	{
		if (!Inventory.Contains(ClothingItem->GetItemType()))
		{
			Inventory.Add(ClothingItem->GetItemType());
		}
	}

	while (Outfit.Num() > 0)
	{
		auto Last = Outfit.Last();
		Outfit.Remove(Last);
		Last->Destroy();
	}
}

FName UInventoryComponent::GetBodySection(FGameplayTag ItemType)
{
	if (ClothingInfo.IsEmpty())
	{
		LoadCollectables();
	}
	if (ClothingInfo.Contains(ItemType))
	{
		return ClothingInfo[ItemType].BodySocket.GetTagName();
	}
	return {};
}