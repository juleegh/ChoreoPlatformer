#include "ClothingItem.h"
#include "ComponentGetters.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

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
	OriginPosition = GetActorLocation();
	OriginScale = GetActorScale();
}

void AClothingItem::OnEnterRange()
{
	if (!bFinished)
	{
		bFinished = true;
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ComponentGetters::GetInventoryComponent(GetWorld())->AddItem(this);
		RefreshState();
	}
}

void AClothingItem::PutBack(FVector NewPosition, bool bToOriginalPosition)
{
	bFinished = false;
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetActorScale3D(OriginScale);
	if (bToOriginalPosition)
	{
		SetActorLocation(OriginPosition);
	}
	else
	{
		SetActorLocation(NewPosition);
	}
	RefreshState();
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



