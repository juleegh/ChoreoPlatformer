// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCell.h"
#include "Kismet/GameplayStatics.h"
#include "SongTempoComponent.h"
#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "Paper2D/Classes/PaperFlipbookComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"

// Sets default values
AGridCell::AGridCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	Base->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Base;

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetCollisionResponseToAllChannels(ECR_Ignore);
	Flipbook->SetupAttachment(RootComponent);

	HitBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hit Box"));
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetupAttachment(RootComponent);

	ColorTimeline = CreateDefaultSubobject<UColorTimelineComponent>("Color Timeline");
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
	auto PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	SongTempo = PlayerController->GetSongTempoComponent();
	ColorTimeline->Initialize();
	ColorTimeline->AddMesh(HitBox);
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

void AGridCell::PromptTrigger()
{
	ColorTimeline->ChangeColor(TriggerColor);
	ColorTimeline->Blink(2);
}

void AGridCell::PromptDamage()
{
	ColorTimeline->ChangeColor(DamageColor);
	ColorTimeline->Blink(2);
}


