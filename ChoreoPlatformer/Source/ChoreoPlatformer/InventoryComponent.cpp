#include "InventoryComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "DancerHealthComponent.h"

bool UInventoryComponent::HasItem(FGameplayTag ItemType)
{
	return Inventory.Contains(ItemType);
}

void UInventoryComponent::AddItem(FGameplayTag ItemType)
{
	Inventory.Add(ItemType);
	InventoryChanged.Broadcast();
	if (ItemType.GetGameplayTagParents().HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Health"))))
	{
		UDanceUtilsFunctionLibrary::GetDancerHealthComponent(GetWorld())->HealthChanged.Broadcast(true, true);
	}
}

bool UInventoryComponent::RemoveItem(FGameplayTag ItemType)
{
	if (HasItem(ItemType))
	{
		Inventory.Remove(ItemType);
		InventoryChanged.Broadcast();
		return true;
	}
	return false;
}

bool UInventoryComponent::HasHealthItem()
{
	for (FGameplayTag Item : Inventory)
	{
		if (Item.GetGameplayTagParents().HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Health"))))
		{
			return true;
		}
	}
	return false;
}

int UInventoryComponent::HealthItemQuantity()
{
	int count = 0;
	for (FGameplayTag Item : Inventory)
	{
		if (Item.GetGameplayTagParents().HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Health"))))
		{
			count++;
		}
	}
	return count;
}

bool UInventoryComponent::LoseHealthItem()
{
	for (FGameplayTag Item : Inventory)
	{
		if (Item.GetGameplayTagParents().HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Health"))))
		{
			Inventory.Remove(Item);
			return true;
		}
	}
	return false;
}