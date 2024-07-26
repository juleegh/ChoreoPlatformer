#include "ContextualElement.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"
#include "SongTempoComponent.h"
#include "TilemapLevelManager.h"
#include "DanceCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "ChoreoPlayerController.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"
#include "CityMesh.h"

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

float AContextualElement::GetTempoDuration()
{
	return ComponentGetters::GetSongTempoComponent(GetWorld())->GetFrequency() * 0.95f;
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
	Tile = nullptr;
}

void ARotatingAnchor::Rotate(float Direction)
{
	if (!Tile)
	{
		auto TileInfo = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, GetActorLocation());
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
	FVector PlayerPosition = ComponentGetters::GetDanceCharacter(GetWorld())->GetActorLocation();
	FVector Position = GetActorLocation();
	PlayerPosition.Z = 0;
	Position.Z = 0;
	FVector Direction = (Position - PlayerPosition);
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	if (CurrentTile.bForcesDirection)
	{
		Direction = CurrentTile.ForcedDirection;
	}
	Direction.Normalize();
	Direction *= 100;
	FVector NextPosition = GetActorLocation() + Direction;

	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, NextPosition);
	if (!NextTile.HitCell || NextTile.bHitEnemy)
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

AWater::AWater()
{
	WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water Mesh"));
	WaterMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
	WaterMesh->SetupAttachment(RootComponent);
}

void AWater::BeginPlay()
{
	Super::BeginPlay();
	MoveTimeline->Initialize();
	InitialLevel = FMath::CeilToInt(GetActorLocation().Z / 50) * 50;
	FVector current = GetActorLocation();
	current.Z = InitialLevel;
	SetActorLocation(current);
	DanceCharacter = ComponentGetters::GetDanceCharacter(GetWorld());
}

#if WITH_EDITOR
void AWater::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Height = FMath::CeilToInt(Height / 50) * 50;
	WidthX = FMath::CeilToInt(WidthX / 50) * 50;
	WidthY = FMath::CeilToInt(WidthY / 50) * 50;
	WaterMesh->SetRelativeScale3D(FVector(WidthX, WidthY, Height) * ScaleFactor);
	WaterMesh->SetRelativeLocation(FVector(0, 0, -Height - 10));
}
#endif

void AWater::Reset()
{
	FVector current = GetActorLocation();
	if (current.Z != InitialLevel && InitialLevel != 0)
	{
		current.Z = InitialLevel;
		SetActorLocation(current);
		
		for (auto Floater : FloatingActors)
		{
			current = Floater->GetActorLocation();
			current.Z = InitialLevel;
			Floater->MoveToPosition(current, GetTempoDuration());
		}
	}

}

bool AWater::ChangeWaterLevel(float Direction)
{
	float CurrentLevel = GetActorLocation().Z;
	float DancerLocation = DanceCharacter->GetActorLocation().Z;
	auto DancerTile = UDanceUtilsFunctionLibrary::CheckPosition({ DanceCharacter }, DanceCharacter->GetActorLocation());
	bool WillDrown = CurrentLevel + Height + 50 >= DancerLocation && DancerTile.HitCell->GetTileType() != ETempoTile::Floater;
	if (Direction > 0 && (CurrentLevel >= InitialLevel + (MaxLevel * 50) || WillDrown))
	{
		return false;
	}

	if (Direction < 0 && CurrentLevel <= InitialLevel + (MinLevel * 50))
	{
		return false;
	}
	FVector MoveDirection = (FVector::UpVector * Direction).GetSafeNormal() * 50;
	MoveTimeline->Stop(false);
	MoveTimeline->MoveToPosition(GetActorLocation() + MoveDirection, GetTempoDuration());
	for (auto Floater : FloatingActors)
	{
		Floater->MoveToPosition(Floater->GetActorLocation() + MoveDirection, GetTempoDuration());
	}
	for (auto Floater : FloatingTiles)
	{
		Floater->Float(MoveDirection);
	}
	return true;
}

void AWater::ToggleHighlight(bool activated)
{
	for (AWaterButton* ConnectedButton : ConnectedButtons)
	{
		ConnectedButton->ColorTimeline->FadeInDirection(activated);
	}
}

EMoveResult AWater::TriggerInteraction()
{
	return EMoveResult::Blocked;
}

EMoveResult AWaterButton::TriggerInteraction()
{
	if (ConnectedWater->ChangeWaterLevel(Direction))
	{
		return EMoveResult::ActionCompleted;
	}
	return EMoveResult::Blocked;
}

void AWaterButton::ToggleHighlight(bool activated)
{
	ConnectedWater->ToggleHighlight(activated);
}

void AWaterTileAnchor::ToggleHighlight(bool activated)
{

}

void AWaterTileAnchor::Reset()
{
	Tile = nullptr;

	FVector current = GetActorLocation();
	if (current.Z != InitialLevel && InitialLevel != 0)
	{
		current.Z = InitialLevel;
		SetActorLocation(current);
	}
}

void AWaterTileAnchor::Float(FVector Delta)
{
	if (!Tile)
	{
		auto TileInfo = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, GetActorLocation());
		Tile = TileInfo.HitCell;
	}

	MoveTimeline->MoveToPosition(GetActorLocation() + Delta, GetTempoDuration());
	auto TileLocation = Tile->GetActorLocation();
	Tile->MoveToPosition(TileLocation + Delta, GetTempoDuration());
	FTileInfo ThisCell = UDanceUtilsFunctionLibrary::CheckPosition({ this }, TileLocation);
	if (ThisCell.bHitPlayer)
	{
		ComponentGetters::GetDanceCharacter(GetWorld())->MoveTo(TileLocation + Delta + FVector::UpVector * 100.f, GetTempoDuration());
	}
}

AWaterTileAnchor::AWaterTileAnchor()
{
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
}

void AWaterTileAnchor::BeginPlay()
{
	Super::BeginPlay();
	MoveTimeline->Initialize();
	InitialLevel = FMath::CeilToInt(GetActorLocation().Z / 50) * 50;
	FVector current = GetActorLocation();
	current.Z = InitialLevel;
	SetActorLocation(current);
}

