#include "InventoryComponent.h"

bool UInventoryComponent::HasItem(FGameplayTag ItemType)
{
	return Inventory.Contains(ItemType);
}

void UInventoryComponent::AddItem(FGameplayTag ItemType)
{
	Inventory.Add(ItemType);
}

bool UInventoryComponent::RemoveItem(FGameplayTag ItemType)
{
	if (HasItem(ItemType))
	{
		Inventory.Remove(ItemType);
		return true;
	}
	return false;
}