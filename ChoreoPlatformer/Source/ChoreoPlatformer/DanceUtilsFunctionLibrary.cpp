// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceUtilsFunctionLibrary.h"
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



