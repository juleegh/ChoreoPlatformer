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
#include "ChoreoPlayerController.h"
#include "Components/BoxComponent.h"
#include "ComponentGetters.h"

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
	
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	for (auto Actor : Enemies)
	{
		if (auto Enemy = Cast<AEnemy>(Actor))
		{
			Enemy->SetupEnemy();
		}
	}

	TArray<AActor*> GameplayElements;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AContextualElement::StaticClass(), GameplayElements);
	for (auto Actor : GameplayElements)
	{
		if (auto Element = Cast<AContextualElement>(Actor))
		{
			Element->Reset();
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

void ASectionLevelManager::Initialize()
{
	CurrentSection = StartSection;
	auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	SongTempo->SetupTempo(60 / SongBPM);
	PlayCurrentSection();

	Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->SetupToLevel();
	SongTempo->StartTempoCounting();
}

void ASectionLevelManager::CurrentSectionEnd(ASectionStart* NextSection)
{
	if (!NextSection->GetSectionIdentifier().IsValid())
	{
		return;
	}
	CurrentSectionEnd(NextSection->GetSectionIdentifier());
	CurrentSectionStart = NextSection;
}

void ASectionLevelManager::CurrentSectionEnd(FGameplayTag NextSection)
{
	if (!NextSection.IsValid())
	{
		return;
	}
	CurrentSection = NextSection;
	CurrentSectionStart = nullptr;

	const FGameplayTag GTEOL = FGameplayTag::RequestGameplayTag("GameUI.EndOfLevel");
	ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->GoToGameScreen(GTEOL);
	LevelEnd.Broadcast();
}

void ASectionLevelManager::NextSectionStart()
{
	if (CurrentSection.MatchesTag(FGameplayTag::RequestGameplayTag("Level.MainMenu")))
	{
		LevelStart.Broadcast();
		ComponentGetters::GetController(GetWorld())->GoBackToMainMenu();
		return;
	}
	
	if (CurrentSection.IsValid())
	{
		GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(CurrentSectionStart->GetActorLocation());
		ComponentGetters::GetTilemapLevelManager(GetWorld())->LoadMap(CurrentSection);
		ComponentGetters::GetInventoryComponent(GetWorld())->ClearItemsEndOfLevel();
		LevelStart.Broadcast();
	}
}

void ASectionLevelManager::StartFromSection(const FGameplayTag SectionIdentifier)
{
	TArray<AActor*> FoundSections;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASectionStart::StaticClass(), FoundSections);

	for (auto Section : FoundSections)
	{
		if (auto LevelSection = Cast<ASectionStart>(Section))
		{
			if (LevelSection->GetSectionIdentifier() != SectionIdentifier)
			{
				continue;
			}

			CurrentSection = LevelSection->GetSectionIdentifier();
			CurrentSectionStart = LevelSection;
			NextSectionStart();
			return;
		}
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

void ULevelEventsComponent::HandleEvent(FLevelEventInfo EventInfo)
{
	LastEventData = EventInfo;
	ActivateTrigger(EventInfo.EventTrigger);
}

void ULevelEventsComponent::ActivateTrigger(FGameplayTag TriggerTag)
{
	if (LevelEvents->CountdownEvents.Contains(TriggerTag))
	{
		HandleCountdownEvent(TriggerTag);
	}
	if (LevelEvents->Sections.Contains(TriggerTag))
	{
		HandleSectionEvent(TriggerTag);
	}
}

void ULevelEventsComponent::HandleCountdownEvent(FGameplayTag TriggerTag)
{
	if (!Countdowns.Contains(TriggerTag))
	{
		ComponentGetters::GetSongTempoComponent(GetWorld())->AddPauseTempos(LevelEvents->CountdownEvents[TriggerTag]);
	}
}

void ULevelEventsComponent::HandleSectionEvent(FGameplayTag TriggerTag)
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

			ComponentGetters::GetSectionLevelManager(GetWorld())->CurrentSectionEnd(LevelSection);
			return;
		}
	}
	ComponentGetters::GetSectionLevelManager(GetWorld())->CurrentSectionEnd(TriggerTag);
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
	FLevelEventInfo EventInfo;
	EventInfo.EventTrigger = ActorTrigger;
	EventInfo.FlavorTriggers.AddTag(FlavorTrigger);
	ComponentGetters::GetLevelEventsComponent(GetWorld())->HandleEvent(EventInfo);
}


