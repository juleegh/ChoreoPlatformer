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

bool UInventoryComponent::HasItem(FGameplayTag ItemType)
{
	return Inventory.Contains(ItemType);
}

void UInventoryComponent::AddItem(AClothingItem* Item)
{
	Inventory.Add(Item->GetItemType());
	Outfit.Add(Item);
	InventoryChanged.Broadcast();
	if (Item->GetItemType().GetGameplayTagParents().HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Health"))))
	{
		UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->HealthChanged.Broadcast(true, true, false);
	}
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(UDanceUtilsFunctionLibrary::GetDanceCharacter(GetOwner())->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	TransformRules.ScaleRule = EAttachmentRule::KeepWorld;
	Item->AttachToComponent(SkeletalMesh, TransformRules, GetBodySection(Item->GetItemType()));
}

bool UInventoryComponent::RemoveItem(FGameplayTag ItemType)
{
	if (HasItem(ItemType))
	{
		Inventory.RemoveSingle(ItemType);
		InventoryChanged.Broadcast();
		return true;
	}
	return false;
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

FName UInventoryComponent::GetBodySection(FGameplayTag ItemType)
{
	TArray<FClothingItemInfo*> Items;
	ItemsData->GetAllRows<FClothingItemInfo>(TEXT("ContextString"), Items);

	for (auto ItemInfo : Items)
	{
		if (ItemInfo->Identifier == ItemType)
		{
			return ItemInfo->BodySection.GetTagName();
		}
	}
	return {};
}