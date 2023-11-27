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

	BodySocket = FGameplayTag::RequestGameplayTag(TEXT("BodySocket.Hat"));
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



