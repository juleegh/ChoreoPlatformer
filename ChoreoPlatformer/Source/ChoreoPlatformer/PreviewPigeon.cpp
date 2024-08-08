// Fill out your copyright notice in the Description page of Project Settings.

#include "PreviewPigeon.h"
#include "Components/StaticMeshComponent.h"
#include "ComponentGetters.h"
#include "InventoryComponent.h"
#include "Components/SceneCaptureComponent2D.h"

APreviewPigeon::APreviewPigeon()
{
	PrimaryActorTick.bCanEverTick = false;

}

void APreviewPigeon::LoadItemData()
{
	TArray<FClothingItemInfo*> Items;
	ItemsData->GetAllRows<FClothingItemInfo>(TEXT("ContextString"), Items);
	RenderingActors.Add(this);

	for (auto ItemInfo : Items)
	{
		OwnedItems.Add(ItemInfo->Identifier, HasCollectedItem(ItemInfo->Identifier));

		if (!CurrentOutfit.Contains(ItemInfo->BodySocket))
		{
			auto ClothingActor = GetWorld()->SpawnActor<AActor>(ClothesBP);
			RenderingActors.Add(ClothingActor);
			FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
			TransformRules.ScaleRule = EAttachmentRule::KeepWorld;
			ClothingActor->AttachToComponent(GetMesh(), TransformRules, ItemInfo->BodySocket.GetTagName());
			CurrentOutfit.Add(ItemInfo->BodySocket, ClothingActor);
		}
	}

	auto actorComponents = GetComponentsByClass(USceneCaptureComponent2D::StaticClass());
	for (auto comp : actorComponents)
	{
		if (auto Capture = Cast<USceneCaptureComponent2D>(comp))
		{
			Capture->ShowOnlyActors = RenderingActors;
		}
	}
}

bool APreviewPigeon::HasCollectedItem(FGameplayTag ItemType)
{
	return ComponentGetters::GetInventoryComponent(GetWorld())->HasCollectedItem(ItemType);
}

TMap<FGameplayTag, bool>& APreviewPigeon::GetItems()
{
	return OwnedItems;
}

void APreviewPigeon::PutOnItem(FGameplayTag ItemType)
{
	auto ItemInfo = GetClothingItem(ItemType);
	if (!HasCollectedItem(ItemInfo.Identifier))
	{
		return;
	}

	AActor* ClothingActor = CurrentOutfit[ItemInfo.BodySocket];
	auto Clothing = Cast<UStaticMeshComponent>(ClothingActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (Clothing->GetStaticMesh() == ItemInfo.Mesh)
	{
		Clothing->SetStaticMesh(nullptr);
		return;
	}

	Clothing->SetStaticMesh(ItemInfo.Mesh);
}

void APreviewPigeon::Clear()
{
	for (auto ClothingActor : CurrentOutfit)
	{
		auto Clothing = Cast<UStaticMeshComponent>(ClothingActor.Value->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		Clothing->SetStaticMesh(nullptr);
	}
}

FClothingItemInfo& APreviewPigeon::GetClothingItem(FGameplayTag ItemType)
{
	return *ComponentGetters::GetInventoryComponent(GetWorld())->GetClothingInfo(ItemType);
}