// Fill out your copyright notice in the Description page of Project Settings.


#include "DanceUtilsFunctionLibrary.h"

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

