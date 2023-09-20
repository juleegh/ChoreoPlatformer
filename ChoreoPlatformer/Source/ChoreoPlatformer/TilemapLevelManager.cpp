// Fill out your copyright notice in the Description page of Project Settings.

#include "TilemapLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GridCell.h"
#include "Enemy.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "ContextualElement.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"
#include "PaperTileSet.h"
#include "GameplayTagContainer.h"
#include "SongTempoComponent.h"
#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "Components/BoxComponent.h"

void ATilemapLevelManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> LevelManager;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectionLevelManager::StaticClass(), LevelManager);

	for (auto Manager : LevelManager)
	{
		if (auto Level = Cast<ASectionLevelManager>(Manager))
		{
			Level->Initialize();
			LoadMap(Level->GetStartSection());
			break;
		}
	}
}

void ATilemapLevelManager::LoadMap(const FGameplayTag& Level)
{
	TilePool.Append(WorldTiles);
	WorldTiles.Empty();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaperTileMapActor::StaticClass(), FoundActors);

	for (auto TileMapActor : FoundActors)
	{
		auto TileMap = Cast<APaperTileMapActor>(TileMapActor)->GetRenderComponent()->TileMap;
		auto FirstLayer = TileMap->TileLayers.Last();
		auto LayerName = FName(FirstLayer->LayerName.ToString());
		FGameplayTag SectionIdentifier = FGameplayTag::RequestGameplayTag(LayerName, false);
		if (!SectionIdentifier.IsValid() || SectionIdentifier != Level)
		{
			continue;
		}

		auto LayerWidth = FirstLayer->GetLayerWidth();
		auto LayerHeight = FirstLayer->GetLayerHeight();

		float LayerPos = 0;
		for (auto LayerInfo : TileMap->TileLayers)
		{
			for (int column = 0; column < LayerWidth; column++)
			{
				for (int row = 0; row < LayerHeight; row++)
				{
					FString TypeT = "";
					auto TileInfo = LayerInfo->GetCell(column, row);

					if (TileInfo.TileSet == nullptr)
						continue;

					auto TileType = TileInfo.PackedTileIndex;
					const FVector DeltaPos = TileMapActor->GetActorLocation() + GetActorRightVector() * row * TileInfo.TileSet->GetTileSize().X + GetActorForwardVector() * column * TileInfo.TileSet->GetTileSize().Y + GetActorUpVector() * LayerPos;

					SpawnTile(DeltaPos, (ETempoTile)TileType, SectionIdentifier);
				}
			}
			LayerPos -= 50.f;
		}

		TileMapActor->SetActorHiddenInGame(true);
	}

	TotalChallenges.Add(EChallengeType::HalfCoin, 0);
	TotalChallenges.Add(EChallengeType::CoinTrail, 0);
	CollectedChallenges.Add(EChallengeType::HalfCoin, 0);
	CollectedChallenges.Add(EChallengeType::CoinTrail, 0);

	TArray<AActor*> FoundChallenges;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileChallenge::StaticClass(), FoundChallenges);

	for (auto Challenge : FoundChallenges)
	{
		if (auto TrailChallenge = Cast<ACoinTrail>(Challenge))
		{
			TotalChallenges[EChallengeType::CoinTrail]++;
		}
		else if (auto HalfChallenge = Cast<AHalfCoin>(Challenge))
		{
			TotalChallenges[EChallengeType::HalfCoin]++;
		}
	}
}

void ATilemapLevelManager::SpawnTile(FVector Position, ETempoTile TileType, FGameplayTag SectionIdentifier)
{
	AGridCell* Current;
	if (TilePool.Num() == 0)
	{
		auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(TileBP, Position, GetActorRotation());
		SpawnedTile->SetOwner(this);
		Current = SpawnedTile;
	}
	else
	{
		Current = TilePool[0];
		Current->SetActorLocation(Position);
		TilePool.RemoveAt(0);
	}
	Current->Initialize(TileType, SectionIdentifier);
	WorldTiles.Add(Current);
}


void ATilemapLevelManager::CollectChallenge(EChallengeType ChallengeType)
{
	CollectedChallenges[ChallengeType]++;
}

int ATilemapLevelManager::GetTotalByChallengeType(EChallengeType ChallengeType)
{
	return TotalChallenges[ChallengeType];
}

int ATilemapLevelManager::GetCollectedByChallengeType(EChallengeType ChallengeType)
{
	return CollectedChallenges[ChallengeType];
}

int ATilemapLevelManager::GetTotalFruit()
{
	return TotalFruit;
}

void ASectionLevelManager::Initialize()
{
	SectionChanged(StartSection);
	auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	SongTempo->SetupTempo(60 / SongBPM);
	PlayCurrentSection();

	auto DanceCharacter = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	DanceCharacter->SetupToLevel();
	SongTempo->StartTempoCounting();
}

void ASectionLevelManager::SectionChanged(FGameplayTag NewSection)
{
	if (!NewSection.IsValid())
	{
		return;
	}

	if (!CurrentSection.IsValid() || CurrentSection != NewSection)
	{
		CurrentSection = NewSection;
		auto LevelEvents = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetEventsComponent();
		LevelEvents->ActivateTrigger(NewSection);
	}
}

ULevelEventsComponent::ULevelEventsComponent()
{
	static ConstructorHelpers::FObjectFinder<UEventsDataAsset>DataAsset(TEXT("/Game/Events/LevelEvents"));
	if (DataAsset.Succeeded())
	{
		LevelEvents = DataAsset.Object;
	}
}

void ULevelEventsComponent::ActivateTrigger(FGameplayTag TriggerTag)
{
	if (LevelEvents->EndTags.Contains(TriggerTag))
	{
		auto SongTempo = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetSongTempoComponent();
		SongTempo->StopTempoCounting();
	}
	if (LevelEvents->WidgetEvents.Contains(TriggerTag))
	{
		HandleWidgetEvent(TriggerTag);
	}
	if (LevelEvents->CountdownEvents.Contains(TriggerTag))
	{
		HandleCountdownEvent(TriggerTag);
	}
	if (LevelEvents->Sections.Contains(TriggerTag))
	{
		HandleSectionEvent(TriggerTag);
	}
}

void ULevelEventsComponent::HandleWidgetEvent(FGameplayTag TriggerTag)
{
	auto EventInfo = LevelEvents->WidgetEvents[TriggerTag];
	if (EventInfo.bSpawnsWidget)
	{
		if (!Widgets.Contains(EventInfo.LevelWidget))
		{
			Widgets.Add(EventInfo.LevelWidget, CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), EventInfo.LevelWidget));
			Widgets[EventInfo.LevelWidget]->AddToViewport();
			Widgets[EventInfo.LevelWidget]->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (Widgets.Contains(EventInfo.LevelWidget))
		{
			Widgets[EventInfo.LevelWidget]->RemoveFromViewport();
			Widgets.Remove(EventInfo.LevelWidget);
		}
	}
}

void ULevelEventsComponent::HandleCountdownEvent(FGameplayTag TriggerTag)
{
	if (!Countdowns.Contains(TriggerTag))
	{
		auto SongTempo = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetSongTempoComponent();
		SongTempo->AddPauseTempos(LevelEvents->CountdownEvents[TriggerTag]);
	}
}

void ULevelEventsComponent::HandleSectionEvent(FGameplayTag TriggerTag)
{
	if (!Sections.Contains(TriggerTag))
	{
		TArray<AActor*> FoundSections;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectionStart::StaticClass(), FoundSections);

		for (auto Section : FoundSections)
		{
			if (auto LevelSection = Cast<ASectionStart>(Section))
			{
				if (LevelSection->GetSectionIdentifier() != TriggerTag)
				{
					continue;
				}
				
				auto TilemapLevelManager = UDanceUtilsFunctionLibrary::GetTilemapLevelManager(GetWorld());
				TilemapLevelManager->LoadMap(LevelSection->GetSectionIdentifier());
				
				auto DanceCharacter = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
				DanceCharacter->SetActorLocation(LevelSection->GetActorLocation());

				auto SectionManager = UDanceUtilsFunctionLibrary::GetSectionLevelManager(GetWorld());
				SectionManager->SectionChanged(LevelSection->GetSectionIdentifier());
				return;
			}
		}
	}
}

AEventTrigger::AEventTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxComponent;
}

void AEventTrigger::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEventTrigger::OnOverlapRangeBegin);
}

void AEventTrigger::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto LevelEvents = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController())->GetEventsComponent();
	LevelEvents->ActivateTrigger(ActorTrigger);
}


