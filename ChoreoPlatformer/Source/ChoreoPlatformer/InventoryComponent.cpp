#include "InventoryComponent.h"
#include "Engine/DataTable.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ComponentGetters.h"
#include "DanceUtilsFunctionLibrary.h"

UInventoryComponent::UInventoryComponent()
{
	ItemsData = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("DataTable'/Game/TileElements/ItemDataTable.ItemDataTable'")));
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadCollectables();
}

void UInventoryComponent::LoadCollectables()
{
	TArray<FClothingItemInfo*> Items;
	ItemsData->GetAllRows<FClothingItemInfo>(TEXT("ContextString"), Items);
	for (auto ItemInfo : Items)
	{
		ClothingInfo.Add(ItemInfo->Identifier, *ItemInfo);
	}
	LoadCollectablesData();
}

void UInventoryComponent::SaveCollectablesData()
{
	UCollectablesData* SaveGameInstance = Cast<UCollectablesData>(UGameplayStatics::CreateSaveGameObject(UCollectablesData::StaticClass()));
	if (SaveGameInstance)
	{
		for (auto ClothingItem : Inventory)
		{
			SaveGameInstance->Collected.Add(ClothingItem.ToString());
		}

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
	}

}

void UInventoryComponent::LoadCollectablesData()
{
	UCollectablesData* LoadedData = Cast<UCollectablesData>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot"), 0));
	if (LoadedData)
	{
		Inventory.Empty();
		for (auto ClothingItem : LoadedData->Collected)
		{
			FGameplayTag Collected = FGameplayTag::RequestGameplayTag(FName(ClothingItem));
			Inventory.Add(Collected);
		}
	}
}

void UInventoryComponent::AddItem(AClothingItem* Item)
{
	Outfit.Add(Item);
	InventoryChanged.Broadcast(true);
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(ComponentGetters::GetDanceCharacter(GetWorld())->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
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
	InventoryChanged.Broadcast(false);
	Last->PutBack(UDanceUtilsFunctionLibrary::GetAvailablePosition(ComponentGetters::GetDanceCharacter(GetWorld()), 2));
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
	SaveCollectablesData();

	while (Outfit.Num() > 0)
	{
		auto Last = Outfit.Last();
		Outfit.Remove(Last);
		Last->PutBack({}, true);
	}
	InventoryCleared.Broadcast();
}

FName UInventoryComponent::GetBodySection(FGameplayTag& ItemType)
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

FClothingItemInfo* UInventoryComponent::GetClothingInfo(FGameplayTag& ItemType)
{
	if (ClothingInfo.IsEmpty())
	{
		LoadCollectables();
	}
	if (ClothingInfo.Contains(ItemType))
	{
		return &ClothingInfo[ItemType];
	}
	return {};
}

bool UInventoryComponent::HasCollectedItem(FGameplayTag& ItemType)
{
	return Inventory.Contains(ItemType);
}
