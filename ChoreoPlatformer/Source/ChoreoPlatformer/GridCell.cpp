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

	FlipbookBackground = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("FlipbookBG"));
	FlipbookBackground->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlipbookBackground->SetupAttachment(Flipbook);
	FlipbookBackground->SetRelativeLocation(-FVector::RightVector);

	SpritesTimeline = CreateDefaultSubobject<USpritesTimelineComponent>("Sprites Timeline");
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
}

void AGridCell::Initialize(ETempoTile Properties, FGameplayTag& BelongingSection)
{
	TileType = Properties;
	Section = BelongingSection;
	ToggleStaticTrigger(ExitType, Properties == ETempoTile::Ending);
	if (TempoFlipbooks.Contains(Properties))
	{
		Flipbook->SetFlipbook(TempoFlipbooks[Properties]);
	}
	Flipbook->SetPlayRate(1.0 / ComponentGetters::GetSongTempoComponent(GetWorld())->GetFrequency());
	Flipbook->PlayFromStart();
	UpdateFlipbookVisuals();
}


void AGridCell::BeginPlay()
{
	Super::BeginPlay();
	SongTempo = ComponentGetters::GetSongTempoComponent(GetWorld());
	ComponentGetters::GetDanceCharacter(GetWorld())->PlayerNewPosition.AddDynamic(this, &AGridCell::UpdateFlipbookVisuals);
	if (auto SectionLevelManager = ComponentGetters::GetSectionLevelManager(GetWorld()))
	{
		SectionLevelManager->LevelStart.AddDynamic(this, &AGridCell::StartFlipbook);
		SectionLevelManager->LevelEnd.AddDynamic(this, &AGridCell::StopFlipbook);
	}
	SpritesTimeline->Initialize();
	MoveTimeline->Initialize();
	GetSprites();
	SpritesTimeline->SetSprites(HitSprites);
	ForegroundMat = Flipbook->CreateDynamicMaterialInstance(0, Flipbook->GetMaterial(0));
	BackgroundMat = FlipbookBackground->CreateDynamicMaterialInstance(0, FlipbookBackground->GetMaterial(0));
}

void AGridCell::UpdateFlipbookVisuals()
{
	float PercentageDistance = (ShineDistance - (float)UDanceUtilsFunctionLibrary::TilesAwayFromPlayer(this)) / ShineDistance;
	ForegroundMat->SetScalarParameterValue(FName("Opacity"), PercentageDistance < MinForeground ? MinForeground : PercentageDistance);
	BackgroundMat->SetScalarParameterValue(FName("Opacity"), PercentageDistance < MinBackground ? MinBackground : PercentageDistance);
}

void AGridCell::StopFlipbook()
{
	Flipbook->Stop();
}

void AGridCell::StartFlipbook()
{
	Flipbook->PlayFromStart();
}

bool AGridCell::ForcesPlayerPosition()
{
	return TileType == ETempoTile::ForceDirection || TileType == ETempoTile::ForcedChangeable;
}

FVector AGridCell::ForcedDirection()
{
	switch (TileType)
	{
	case ETempoTile::ForceDirection:
	case ETempoTile::ForcedChangeable:
		return FVector(FMath::RoundToInt(GetActorForwardVector().X), FMath::RoundToInt(GetActorForwardVector().Y), 0);
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
	SpritesTimeline->ChangeType(SpritesInfo[ItemType]);
	SpritesTimeline->Blink();
}

void AGridCell::PromptTrigger()
{
	SpritesTimeline->ChangeType(SpritesInfo[TriggerType]);
	SpritesTimeline->Blink();
}

void AGridCell::PromptDamage()
{
	SpritesTimeline->ChangeType(SpritesInfo[DamageType]);
	SpritesTimeline->Blink();
}

void AGridCell::RotateToDirection(FRotator Direction)
{
	if (MoveTimeline->IsRunning())
	{
		MoveTimeline->Stop(true);
	}
	MoveTimeline->RotateToPosition(GetActorRotation() + Direction, ComponentGetters::GetSongTempoComponent(GetWorld())->GetFrequency());
	PromptTrigger();
}

void AGridCell::ToggleStaticTrigger(const FGameplayTag& SpriteType, bool bVisible)
{
	auto Properties = SpritesInfo[SpriteType];
	Properties.Color.A = bVisible;
	SpritesTimeline->ChangeType(Properties);
	SpritesTimeline->ChangeBrightness(bVisible ? 1 : 0.2f);
}
