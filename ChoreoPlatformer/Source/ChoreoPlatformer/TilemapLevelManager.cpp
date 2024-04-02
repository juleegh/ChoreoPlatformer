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
#include "TimerManager.h"

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

		LoadTileMap(TileMap, TileMapActor->GetActorLocation(), SectionIdentifier);
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

void ATilemapLevelManager::LoadTileMap(const UPaperTileMap* TileMap, FVector AnchorLocation, FGameplayTag SectionIdentifier)
{
	auto FirstLayer = TileMap->TileLayers.Last();

	auto LayerWidth = FirstLayer->GetLayerWidth();
	auto LayerHeight = FirstLayer->GetLayerHeight();

	float LayerPos = 0;
	for (auto LayerInfo : TileMap->TileLayers)
	{
		for (int column = 0; column < LayerWidth; column++)
		{
			for (int row = 0; row < LayerHeight; row++)
			{
				auto TileInfo = LayerInfo->GetCell(column, row);

				if (TileInfo.TileSet == nullptr || TileInfo.TileSet != TileSet.Get())
					continue;

				auto TileType = TileInfo.PackedTileIndex;

				FRotator DeltaRotation = FRotator();
				DeltaRotation.Yaw = 90 * TileInfo.GetFlagsAsIndex() - 90;

				const FVector DeltaPos = AnchorLocation + GetActorRightVector() * row * TileInfo.TileSet->GetTileSize().X + GetActorForwardVector() * column * TileInfo.TileSet->GetTileSize().Y + GetActorUpVector() * LayerPos;

				SpawnTile(DeltaPos, DeltaRotation, (ETempoTile)TileType, SectionIdentifier);
			}
		}
		LayerPos -= 50.f;
	}
}

void ATilemapLevelManager::SpawnTile(FVector Position, FRotator DeltaRotation, ETempoTile TileType, FGameplayTag SectionIdentifier)
{
	AGridCell* Current;
	if (TilePool.Num() == 0)
	{
		auto SpawnedTile = GetWorld()->SpawnActor<AGridCell>(TileBP, Position, DeltaRotation);
		SpawnedTile->SetOwner(this);
		Current = SpawnedTile;
	}
	else
	{
		Current = TilePool[0];
		Current->SetActorLocation(Position);
		Current->SetActorRotation(DeltaRotation);
		TilePool.RemoveAt(0);
		Current->SetActorHiddenInGame(false);
	}
	Current->Initialize(TileType, SectionIdentifier);
	WorldTiles.Add(Current);
}

void ATilemapLevelManager::ClearTile(AGridCell* ClearingTile)
{
	ClearingTile->SetActorLocation(FVector::ForwardVector * -1000);
	WorldTiles.Remove(ClearingTile);
	TilePool.Add(ClearingTile);
}

TArray<AGridCell*>* ATilemapLevelManager::GetWorldTiles()
{
	return &WorldTiles;
}

void ASectionLevelManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentSection = StartSection;
	auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	SongTempo->SetupTempo(60 / SongBPM);

	Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->InitializeToLevel(60 / SongBPM);
	SongTempo->StartTempoCounting();

	ComponentGetters::GetDanceAudioManager(GetWorld())->InitializeSong();
	ComponentGetters::GetTilemapLevelManager(GetWorld())->LoadMap(CurrentSection);
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
	float DelayDuration = 0.25f;
	FTimerDelegate TimerCallback;

	TimerCallback.BindLambda([this]() {
		// Your lambda function code here
		ComponentGetters::GetDanceCharacter(GetWorld())->SetupPlayerCamera(ComponentGetters::GetLevelEventsComponent(GetWorld())->GetLastEventData().FlavorTriggers.First());
		});

	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, TimerCallback, DelayDuration, false);

	LevelEnd.Broadcast();
}

void ASectionLevelManager::NextSectionStart()
{
	ComponentGetters::GetInventoryComponent(GetWorld())->ClearItemsEndOfLevel();
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
		ComponentGetters::GetDanceCharacter(GetWorld())->SetupPlayerCamera(FGameplayTag::EmptyTag);
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
	if (TriggerTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Level"))))
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

void AEndlessLevelManager::Initialize()
{
	auto SongTempo = GetWorld()->GetFirstPlayerController()->FindComponentByClass<USongTempoComponent>();
	SongTempo->SetupTempo(60 / InitialSongBPM);

	Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->InitializeToLevel(60 / InitialSongBPM);
	SongTempo->StartTempoCounting();

	auto TileMap = AvailableTileMaps[FMath::RandRange(0, AvailableTileMaps.Num() - 1)];
	ComponentGetters::GetTilemapLevelManager(GetWorld())->LoadTileMap(TileMap, FVector::Zero(), FGameplayTag::EmptyTag);

	LoadNextMap(TileMap);
	CurrentTiles = NextTiles;
	/*
	*/
}

void AEndlessLevelManager::ShuffleWorldDown(float ElapsedTime)
{
	CurrentLine++;
	// if the line reaches the top of the current level load the next one and adjust indexes
	if (CurrentLine >= TileMapDimension + (TileMapDimension / 2))
	{
		CurrentTiles = NextTiles;
		auto TileMap = AvailableTileMaps[FMath::RandRange(0, AvailableTileMaps.Num() - 1)];
		LoadNextMap(TileMap);
		CurrentLine = TileMapDimension / 2;
	}

	auto TileManager = ComponentGetters::GetTilemapLevelManager(GetWorld());
	// Clear current line
	for (int i = 0; i < TileMapDimension; i++)
	{
		FVector Pos = FVector::RightVector * i * TileDimension;
		auto TileInfo = UDanceUtilsFunctionLibrary::CheckPosition({ ComponentGetters::GetDanceCharacter(GetWorld()) }, Pos);
		if (TileInfo.HitCell == nullptr)
		{
			continue;
		}
		TileManager->ClearTile(TileInfo.HitCell);
	}

	// Spawn next line
	int NextLine = CurrentLine - TileMapDimension / 2;
	for (int row = 0; row < TileMapDimension; row++)
	{
		auto Tile = NextTiles[FVector::ForwardVector * NextLine + FVector::RightVector * row];
		FRotator DeltaRotation = FRotator::MakeFromEuler(Tile.ForcedDirection);
		const FVector DeltaPos = FVector::ForwardVector * (TileMapDimension)*TileDimension + FVector::RightVector * row * TileDimension;

		TileManager->SpawnTile(DeltaPos, DeltaRotation, Tile.TileType, FGameplayTag::EmptyTag);
	}

	// Shuffle downwards
	auto WorldTiles = TileManager->GetWorldTiles();
	for (auto Tile : *WorldTiles)
	{
		Tile->MoveToPosition(Tile->GetActorLocation() - FVector::ForwardVector * TileDimension, ElapsedTime);
		Tile->UpdateFlipbookVisuals();
	}
}

void AEndlessLevelManager::LoadNextMap(UPaperTileMap* TileMap)
{
	NextTiles.Empty();
	auto FirstLayer = TileMap->TileLayers.Last();

	TileMapDimension = FirstLayer->GetLayerWidth();

	for (auto LayerInfo : TileMap->TileLayers)
	{
		for (int row = 0; row < TileMapDimension; row++)
		{
			TArray<FProceduralTileInfo> CurrentRow;
			for (int column = 0; column < TileMapDimension; column++)
			{
				FProceduralTileInfo CurrentTile;
				auto TileInfo = LayerInfo->GetCell(column, row);

				if (TileInfo.TileSet == nullptr)
				{
					CurrentTile.TileType = ETempoTile::Empty;
				}
				else
				{
					TileDimension = TileInfo.TileSet->GetTileSize().X;
					CurrentTile.TileType = (ETempoTile)TileInfo.PackedTileIndex;
					FRotator DeltaRotation = FRotator();
					DeltaRotation.Yaw = 90 * TileInfo.GetFlagsAsIndex() - 90;
					CurrentTile.ForcedDirection = DeltaRotation.Euler();
				}
				NextTiles.Add(FVector::ForwardVector * column + FVector::RightVector * row, CurrentTile);
			}
		}
	}
}

void AEndlessLevelManager::PlayerMoved(FVector Delta)
{
	CurrentLine += Delta.X;
}

bool AEndlessLevelManager::ShouldShuffleWorldInstead(FVector NextPosition)
{
	return NextPosition.X / TileDimension >= TileMapDimension / 2;
}
