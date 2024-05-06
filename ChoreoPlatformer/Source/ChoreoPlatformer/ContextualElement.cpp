#include "ContextualElement.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"
#include "TilemapLevelManager.h"
#include "DanceCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "ChoreoPlayerController.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"

AContextualElement::AContextualElement()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxComponent;

	InteractionHighlight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interaction Highlight"));
	InteractionHighlight->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionHighlight->SetupAttachment(RootComponent);

	ColorTimeline = CreateDefaultSubobject<UColorFadeTimelineComponent>("Color Timeline");
}

void AContextualElement::BeginPlay()
{
	Super::BeginPlay();
	ColorTimeline->Initialize();
	ColorTimeline->AddMesh(InteractionHighlight);
	ColorTimeline->Reset();
}

void AContextualElement::ToggleHighlight(bool activated)
{
	ColorTimeline->FadeInDirection(activated);
}

void AContextualElement::Reset()
{
	bFinished = false;
	OnReset();
}

EMoveResult ABrickWall::TriggerInteraction()
{
	HitsLeft--;
	if (HitsLeft <= 0)
	{
		bFinished = true;
		RefreshState();
	}
	return EMoveResult::None;
}

void ADoor::ToggleHighlight(bool activated)
{
	if (BelongingLever)
	{
		BelongingLever->ToggleHighlight(activated);
	}
}

void ADoor::Open()
{
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	bFinished = true;
	RefreshState();
}

void ADoor::Reset()
{
	Super::Reset();
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

EMoveResult ADoor::TriggerInteraction()
{
	return EMoveResult::Blocked;
}

void ALever::BeginPlay()
{
	Super::BeginPlay();
	for (ADoor* ConnectedDoor : ConnectedDoors)
	{
		ConnectedDoor->BelongingLever = this;
	}
}

void ALever::ToggleHighlight(bool activated)
{
	if (bFinished)
	{
		return;
	}
	ColorTimeline->FadeInDirection(activated);
	for (ADoor* ConnectedDoor : ConnectedDoors)
	{
		ConnectedDoor->ColorTimeline->FadeInDirection(activated);
	}
}

EMoveResult ALever::TriggerInteraction()
{
	if (ConnectedDoors.Num() > 0)
	{
		RefreshState();
		ToggleHighlight(false);
		for (ADoor* ConnectedDoor : ConnectedDoors)
		{
			ConnectedDoor->Open();
			auto DoorTile = UDanceUtilsFunctionLibrary::CheckPosition({ ConnectedDoor }, ConnectedDoor->GetActorLocation());
			if (DoorTile.HitCell)
			{
				DoorTile.HitCell->PromptTrigger();
			}
		}
		bFinished = true;
		return EMoveResult::ActionCompleted;
	}
	return EMoveResult::None;
}

void ARotatingAnchor::ToggleHighlight(bool activated)
{
	if (BelongingLever)
	{
		BelongingLever->ToggleHighlight(activated);
	}
}

void ARotatingAnchor::Reset()
{
	if (Tile)
	{
		Tile->SetActorRotation(InitialRotation);
		Tile = nullptr;
	}
}

void ARotatingAnchor::Rotate(float Direction)
{
	if (!Tile)
	{
		auto TileInfo = UDanceUtilsFunctionLibrary::CheckPosition({this, ComponentGetters::GetDanceCharacter(GetWorld())}, GetActorLocation());
		Tile = TileInfo.HitCell;
		InitialRotation = Tile->GetActorRotation();
	}
	Tile->RotateToDirection(FRotator(0, Direction, 0));
}

void ARotationButton::BeginPlay()
{
	Super::BeginPlay();
	for (ARotatingAnchor* ConnectedTile : ConnectedTiles)
	{
		ConnectedTile->BelongingLever = this;
	}
}

void ARotationButton::ToggleHighlight(bool activated)
{
	ColorTimeline->FadeInDirection(activated);
	for (ARotatingAnchor* ConnectedTile : ConnectedTiles)
	{
		ConnectedTile->ColorTimeline->FadeInDirection(activated);
	}
}

EMoveResult ARotationButton::TriggerInteraction()
{
	if (ConnectedTiles.Num() > 0)
	{
		RefreshState();
		for (ARotatingAnchor* ConnectedTile : ConnectedTiles)
		{
			ConnectedTile->Rotate(Direction);
		}
		return EMoveResult::ActionCompleted;
	}
	return EMoveResult::None;
}

void ATileHole::ToggleHighlight(bool activated)
{
	if (bFinished)
		return;
	ColorTimeline->FadeInDirection(activated);
	TArray<AActor*> FoundTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlacingTile::StaticClass(), FoundTiles);
	for (AActor* ConnectedTile : FoundTiles)
	{
		if (auto Tile = Cast<APlacingTile>(ConnectedTile))
		{
			if (Tile->CanInteract())
			{
				Tile->ColorTimeline->FadeInDirection(activated);
			}
		}
	}
}

EMoveResult ATileHole::TriggerInteraction()
{
	return EMoveResult::Blocked;
}

void ATileHole::CoverHole()
{
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ToggleHighlight(false);
	bFinished = true;
	RefreshState();
}

void ATileHole::Reset()
{
	Super::Reset();
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void APlacingTile::BeginPlay()
{
	Super::BeginPlay();
	InitialPosition = GetActorLocation();
}

void APlacingTile::ToggleHighlight(bool activated)
{
	if (bFinished)
		return;
	ColorTimeline->FadeInDirection(activated);
	TArray<AActor*> FoundTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileHole::StaticClass(), FoundTiles);
	for (AActor* ConnectedTile : FoundTiles)
	{
		if (auto Tile = Cast<ATileHole>(ConnectedTile))
		{
			if (Tile->CanInteract())
			{
				Tile->ColorTimeline->FadeInDirection(activated);
			}
		}
	}
}

EMoveResult APlacingTile::TriggerInteraction()
{
	FVector Direction = (GetActorLocation() - ComponentGetters::GetDanceCharacter(GetWorld())->GetActorLocation());
	Direction.Normalize();
	Direction *= 100;
	FVector NextPosition = GetActorLocation() + Direction;
	
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, NextPosition);
	if (!NextTile.HitCell)
	{
		NextPosition = GetActorLocation() - Direction;
		NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, NextPosition);
	}
	SetActorLocation(NextTile.HitCell->GetActorLocation());

	if (auto Hole = Cast<ATileHole>(NextTile.HitElement))
	{
		Hole->CoverHole();
		BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		bFinished = true;
		RefreshState();
	}
	
	return EMoveResult::ObjectMoved;
}

void APlacingTile::Reset()
{
	Super::Reset();
	if (InitialPosition != FVector::Zero())
	{
		SetActorLocation(InitialPosition);
	}
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

