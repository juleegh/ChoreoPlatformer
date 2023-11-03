// Fill out your copyright notice in the Description page of Project Settings.

#include "DanceUtilsFunctionLibrary.h"
#include "TilemapLevelManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "DanceCharacter.h"
#include "Interface_Highlighter.h"
#include "DancerHealthComponent.h"
#include "SongTempoComponent.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ContextualElement.h"

float UDanceUtilsFunctionLibrary::GetTargetTempo(ETempoTile TileType)
{
	switch (TileType)
	{
	case ETempoTile::Half:
		return 0.5f;
	}

	return 1;
}

FVector UDanceUtilsFunctionLibrary::GetTransformedPosition(FVector Origin, FVector Direction)
{
	return Origin + Direction * 100;
}

ETempoAccuracy UDanceUtilsFunctionLibrary::GetTempoResult(float Distance)
{
	if (Distance <= GetPerfectAcceptanceRate())
	{
		return ETempoAccuracy::Perfect;
	}
	else if (Distance <= GetAcceptanceRate())
	{
		return ETempoAccuracy::Great;
	}
	return ETempoAccuracy::Bad;
}

void UDanceUtilsFunctionLibrary::ToggleHighlight(AActor* Target, bool Highlighted)
{
	if (IInterface_Highlighter* Mesh = Cast<IInterface_Highlighter>(Target))
	{
		Mesh->ToggleHighlight(Highlighted);
	}
}

FTileInfo UDanceUtilsFunctionLibrary::CheckPosition(TArray<AActor*> ToIgnore, FVector Start)
{
	TArray<struct FHitResult> OutHits = {};
	FTileInfo DetectedInfo;

	Start.Z = 500.f;

	FVector End = ((-FVector::ZAxisVector * 510) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetDanceCharacter(ToIgnore[0]));
	for (auto ignore : ToIgnore)
	{
		CollisionParams.AddIgnoredActor(ignore);
	}

	//DrawDebugLine(ToIgnore->GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);

	if (ToIgnore[0]->GetWorld()->LineTraceMultiByChannel(OutHits, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetActor()->GetName()));
		for (auto hit : OutHits)
		{
			if (auto element = Cast<AContextualElement>(hit.GetActor()))
			{
				DetectedInfo.bHitElement = true;
				DetectedInfo.Position = element->GetActorLocation();
				DetectedInfo.HitElement = element;
			}
			
			if (auto gridCell = Cast<AGridCell>(hit.GetActor()))
			{
				DetectedInfo.HitCell = gridCell;
				DetectedInfo.TileType = gridCell->GetTileType();
				DetectedInfo.Position = gridCell->GetActorLocation() + FVector::ZAxisVector * 100;
				DetectedInfo.TargetTempo = UDanceUtilsFunctionLibrary::GetTargetTempo(gridCell->GetTileType());
				DetectedInfo.bForcesDirection = gridCell->ForcesPlayerPosition();
				DetectedInfo.ForcedDirection = gridCell->ForcedDirection();
				DetectedInfo.Section = gridCell->GetSection();
			}
		}
	}

	return DetectedInfo;
}

TArray<AActor*> UDanceUtilsFunctionLibrary::GetAdjacent(AActor* ToIgnore, FVector Position, int Radius)
{
	TArray<AActor*> Elements = {};
	TArray<struct FHitResult> OutHits = {};

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(ToIgnore);

	if (ToIgnore->GetWorld()->SweepMultiByChannel(OutHits, Position, Position, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeBox(FVector(Radius * 100)), CollisionParams))
	{
		for (auto hit : OutHits)
		{
			if (auto element = Cast<AContextualElement>(hit.GetActor()))
			{
				Elements.Add(hit.GetActor());
			}

			if (auto element = Cast<AClothingItem>(hit.GetActor()))
			{
				Elements.Add(hit.GetActor());
			}
		}
	}

	return Elements;
}

bool UDanceUtilsFunctionLibrary::PositionsAreEqual(FVector pos1, FVector pos2)
{
	int x1 = pos1.X;
	int x2 = pos2.X;
	int y1 = pos1.Y;
	int y2 = pos2.Y;
	return x1 == x2 && y1 == y2;
}

float UDanceUtilsFunctionLibrary::GetHealthDelta(ETempoAccuracy result)
{
	switch (result)
	{
	case ETempoAccuracy::Bad:
		return -0.25f;
	case ETempoAccuracy::Great:
		return 0.05f;
	case ETempoAccuracy::Perfect:
		return 0.1f;
	}

	return 0;
}

bool UDanceUtilsFunctionLibrary::IsAdjacentToPlayer(AActor* WorldActor, int TilesAway)
{
	auto Player = WorldActor->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	auto Actor = WorldActor->GetActorLocation();
	float Distance = TilesAway * 100;

	return Player.X <= Actor.X + Distance && Player.X >= Actor.X - Distance && Player.Y <= Actor.Y + Distance && Player.Y >= Actor.Y - Distance;
}

ADanceCharacter* UDanceUtilsFunctionLibrary::GetDanceCharacter(AActor* WorldContextActor)
{
	auto DanceController = WorldContextActor->GetWorld()->GetFirstPlayerController();
	return Cast<ADanceCharacter>(DanceController->GetPawn());
}

UDancerHealthComponent* UDanceUtilsFunctionLibrary::GetDancerHealthComponent(UWorld* WorldContextObject)
{
	return Cast<UDancerHealthComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UDancerHealthComponent::StaticClass()));
}

UInventoryComponent* UDanceUtilsFunctionLibrary::GetInventoryComponent(AActor* WorldContextActor)
{
	return Cast<UInventoryComponent>(WorldContextActor->GetWorld()->GetFirstPlayerController()->GetComponentByClass(UInventoryComponent::StaticClass()));
}

USongTempoComponent* UDanceUtilsFunctionLibrary::GetSongTempoComponent(AActor* WorldContextActor)
{
	return Cast<USongTempoComponent>(WorldContextActor->GetWorld()->GetFirstPlayerController()->GetComponentByClass(USongTempoComponent::StaticClass()));
}

ATilemapLevelManager* UDanceUtilsFunctionLibrary::GetTilemapLevelManager(UWorld* WorldContextObject)
{
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ATilemapLevelManager::StaticClass(), FoundManagers);

	for (auto Manager : FoundManagers)
	{
		return Cast<ATilemapLevelManager>(Manager);
	}

	return nullptr;
}

ASectionLevelManager* UDanceUtilsFunctionLibrary::GetSectionLevelManager(UWorld* WorldContextObject)
{
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ASectionLevelManager::StaticClass(), FoundManagers);

	for (auto Manager : FoundManagers)
	{
		return Cast<ASectionLevelManager>(Manager);
	}

	return nullptr;
}


