// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
#include "ChoreoPlayerController.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "Paper2D/Classes/PaperFlipbookComponent.h"
#include "DanceUtilsFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"
#include "ComponentGetters.h"

AGridCell::AGridCell()
{
	PrimaryActorTick.bCanEverTick = true;

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	Base->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Base;

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetCollisionResponseToAllChannels(ECR_Ignore);
	Flipbook->SetupAttachment(RootComponent);

	SpritesTimeline = CreateDefaultSubobject<USpritesTimelineComponent>("Sprites Timeline");
}

void AGridCell::Initialize(ETempoTile Properties, FGameplayTag& BelongingSection)
{
	TileType = Properties;
	Section = BelongingSection;
	PaintTile();
}


// Called when the game starts or when spawned
void AGridCell::BeginPlay()
{
	Super::BeginPlay();
	SongTempo = ComponentGetters::GetSongTempoComponent(GetWorld());
	SpritesTimeline->Initialize();
	GetSprites();
	SpritesTimeline->SetSprites(HitSprites);
}

// Called every frame
void AGridCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGridCell::ForcesPlayerPosition()
{
	return TileType == ETempoTile::ForceUp ||
		TileType == ETempoTile::ForceDown ||
		TileType == ETempoTile::ForceLeft ||
		TileType == ETempoTile::ForceRight;
}

FVector AGridCell::ForcedDirection()
{
	switch (TileType)
	{
		case ETempoTile::ForceUp:
			return -FVector::RightVector;
		case ETempoTile::ForceDown:
			return FVector::RightVector;
		case ETempoTile::ForceRight:
			return FVector::ForwardVector;
		case ETempoTile::ForceLeft:
			return -FVector::ForwardVector;
		default:
			return FVector::ZeroVector;
	}
}

FGameplayTag& AGridCell::GetSection()
{
	return Section;
}

void AGridCell::PromptItem()
{
	SpritesTimeline->ChangeColor(ItemColor);
	SpritesTimeline->Blink();
}

void AGridCell::PromptTrigger()
{
	SpritesTimeline->ChangeColor(TriggerColor);
	SpritesTimeline->Blink();
}

void AGridCell::PromptDamage()
{
	SpritesTimeline->ChangeColor(DamageColor);
	SpritesTimeline->Blink();
}


