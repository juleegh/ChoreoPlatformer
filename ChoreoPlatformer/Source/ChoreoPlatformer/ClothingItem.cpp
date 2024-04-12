#include "ClothingItem.h"
#include "ComponentGetters.h"
#include "TimelineCreatorComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AClothingItem::AClothingItem()
{
	PrimaryActorTick.bCanEverTick = false;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetupAttachment(BoxComponent);
	ProjectileTimeline = CreateDefaultSubobject<UProjectileTimelineComponent>("Projectile Timeline");
}

void AClothingItem::BeginPlay()
{
	Super::BeginPlay();
	ToggleHighlight(false);
	OriginPosition = GetActorLocation();
	OriginScale = GetActorScale();
	ProjectileTimeline->Initialize();
	ProjectileTimeline->TimelineEnded.AddDynamic(this, &AClothingItem::LandedOnGround);
}

void AClothingItem::OnEnterRange()
{
	if (!bFinished)
	{
		bFinished = true;
		auto CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
		if (CurrentTile.HitCell)
		{
			CurrentTile.HitCell->PromptItem();
		}
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ComponentGetters::GetInventoryComponent(GetWorld())->AddItem(this);
		RefreshState();
		ToggleHighlight(false);
	}
}

void AClothingItem::PutBack(FVector NewPosition, bool bToOriginalPosition)
{
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetActorScale3D(OriginScale);
	if (bToOriginalPosition)
	{
		SetActorLocation(OriginPosition);
		LandedOnGround();
	}
	else
	{
		ProjectileTimeline->Throw(ComponentGetters::GetDanceCharacter(GetWorld())->GetActorLocation(), NewPosition + FVector::UpVector * -50, 1.5f);
	}
}

#if WITH_EDITOR
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
#endif

void AClothingItem::LandedOnGround()
{
	bFinished = false;
	auto CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	if (CurrentTile.HitCell)
	{
		CurrentTile.HitCell->PromptItem();
	}
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	RefreshState();
	ToggleHighlight(UDanceUtilsFunctionLibrary::IsAdjacentToPlayer(this, 1));
}