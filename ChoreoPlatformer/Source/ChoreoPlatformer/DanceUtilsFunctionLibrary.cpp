// Fill out your copyright notice in the Description page of Project Settings.

#include "DanceUtilsFunctionLibrary.h"
#include "TilemapLevelManager.h"
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

FTileInfo UDanceUtilsFunctionLibrary::CheckPosition(AActor* ToIgnore, FVector Start)
{
	FHitResult OutHit;
	FTileInfo DetectedInfo;

	Start.Z -= 50.f;

	FVector DownVector = -FVector::ZAxisVector;
	FVector End = ((DownVector * 100) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(ToIgnore);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);

	if (ToIgnore->GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetActor()->GetName()));
		if (auto actor = Cast<AGridCell>(OutHit.GetActor()))
		{
			DetectedInfo.TileType = actor->GetTileType();
			DetectedInfo.TargetTempo = UDanceUtilsFunctionLibrary::GetTargetTempo(actor->GetTileType());
			DetectedInfo.bForcesDirection = actor->ForcesPlayerPosition();
			DetectedInfo.ForcedDirection = actor->ForcedDirection();
			DetectedInfo.Section = actor->GetSection();
		}
	}

	Start.Z += 50.f;

	DetectedInfo.bHitElement = false;
	if (ToIgnore->GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		if (auto element = Cast<AContextualElement>(OutHit.GetActor()))
		{
			DetectedInfo.bHitElement = true;
			DetectedInfo.HitElement = element;
		}
	}

	return DetectedInfo;
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

bool UDanceUtilsFunctionLibrary::IsAdjacentToPlayer(AActor* WorldActor)
{
	FVector PlayerPos = WorldActor->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector ActorPos = WorldActor->GetActorLocation();

	if (FVector::Distance(PlayerPos, ActorPos) <= 100)
		return true;
	else if (PositionsAreEqual(PlayerPos, ActorPos + FVector::RightVector * 100))
		return true;
	else if (PositionsAreEqual(PlayerPos, ActorPos - FVector::RightVector * 100))
		return true;
	else if (PositionsAreEqual(PlayerPos, ActorPos + FVector::ForwardVector * 100))
		return true;
	else if (PositionsAreEqual(PlayerPos, ActorPos - FVector::ForwardVector * 100))
		return true;

	return false;
}


UDancerHealthComponent* UDanceUtilsFunctionLibrary::GetDancerHealthComponent(UWorld* WorldContextObject)
{
	return Cast<UDancerHealthComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UDancerHealthComponent::StaticClass()));
}

UInventoryComponent* UDanceUtilsFunctionLibrary::GetInventoryComponent(UWorld* WorldContextObject)
{
	return Cast<UInventoryComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UInventoryComponent::StaticClass()));
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


