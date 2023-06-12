// Fill out your copyright notice in the Description page of Project Settings.


#include "TileChallenge.h"
#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "DancerUIComponent.h"
#include "Components/SplineComponent.h"
#include "DanceCharacter.h"
#include "DanceUtilsFunctionLibrary.h"
#include "TilemapLevelManager.h"

ATileChallenge::ATileChallenge()
{
	PrimaryActorTick.bCanEverTick = false;
	InterestPoints = CreateDefaultSubobject<USplineComponent>(TEXT("Patrol Road"));
	InterestPoints->SetupAttachment(RootComponent);
}

void ATileChallenge::BeginPlay()
{
	Super::BeginPlay();
	bUnderProgress = false;
	bCompleted = false;
	Player = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	Player->PlayerNewPosition.AddDynamic(this, &ATileChallenge::PlayerChangedPosition);
	auto PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerUI = PlayerController->GetDancerUIComponent();
	SongTempo = PlayerController->GetSongTempoComponent();
	SongTempo->NewTempoStarted.AddDynamic(this, &ATileChallenge::TempoHasPassed);
}

void ATileChallenge::TempoHasPassed()
{
	if (!IsUndergoing() || bCompleted)
	{
		return;
	}

	TemposWithoutMoving++;
}

int ATileChallenge::GetPlayerCurrentIndex()
{
	for (int Index = 0; Index < InterestPoints->GetNumberOfSplinePoints(); Index++)
	{
		FVector PointLocation = InterestPoints->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
		if (UDanceUtilsFunctionLibrary::PositionsAreEqual(Player->GetActorLocation(), PointLocation))
		{
			return Index;
		}
	}
	return -1;
}

void ATileChallenge::PlayerChangedPosition()
{
	if (bCompleted)
	{
		return;
	}

	if (!bUnderProgress && GetPlayerCurrentIndex() == 0)
	{
		bUnderProgress = true;
		StartChallenge();
		ChallengeStarted();
		PlayerUI->ChallengeStarted(ChallengeType);
	}
	else if (bUnderProgress)
	{
		TemposWithoutMoving--;
		if (GetPlayerCurrentIndex() == InterestPoints->GetNumberOfSplinePoints() - 1)
		{
			EndChallenge(true);
			PlayerUI->ChallengeEnded(ChallengeType, true);
			bUnderProgress = false;
			bCompleted = true;
		}
	}
}

bool ATileChallenge::IsUndergoing()
{
	return bUnderProgress;
}

void ATileChallenge::EndChallenge(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UDanceUtilsFunctionLibrary::GetTilemapLevelManager(GetWorld())->CollectChallenge(ChallengeType);
	}
	ChallengeEnded(bWasSuccessful);
}


void AHalfCoin::StartChallenge()
{
	TemposRemaining = TemposBetweenHalfs;
}

void AHalfCoin::TempoHasPassed()
{
	Super::TempoHasPassed();

	if (bUnderProgress)
	{
		TemposRemaining--;
		ChallengeUpdated();
	}
	if (bUnderProgress && TemposRemaining == 0)
	{
		bUnderProgress = false;
		bCompleted = true;
		EndChallenge(false);
		PlayerUI->ChallengeEnded(ChallengeType, false);
	}
}

void ACoinTrail::StartChallenge()
{
	CurrentCoin = -1;
}

void ACoinTrail::TempoHasPassed()
{
	Super::TempoHasPassed();
	if (!bUnderProgress)
	{
		return;
	}
	if (TemposWithoutMoving >= 2)
	{
		bUnderProgress = false;
		EndChallenge(false);
		PlayerUI->ChallengeEnded(ChallengeType, false);
	}
}

void ACoinTrail::PlayerChangedPosition()
{
	Super::PlayerChangedPosition();

	if (bUnderProgress)
	{
		if (GetPlayerCurrentIndex() == CurrentCoin + 1)
		{
			CurrentCoin++;
			ChallengeUpdated();
		}
		else
		{
			bUnderProgress = false;
			bCompleted = true;
			EndChallenge(false);
			PlayerUI->ChallengeEnded(ChallengeType, false);
		}
	}
}

void ACoinStop::PlayerChangedPosition()
{
	if (bCompleted)
	{
		return;
	}

	if (!bUnderProgress && GetPlayerCurrentIndex() == 0)
	{
		bUnderProgress = true;
		StartChallenge();
		ChallengeStarted();
		PlayerUI->ChallengeStarted(ChallengeType);
		return;
	}

	if (bUnderProgress)
	{
		if (TemposWithoutMoving >= TemposInStop - 1 && SongTempo->TempoResult(1))
		{
			EndChallenge(true);
			PlayerUI->ChallengeEnded(ChallengeType, true);
		}
		else
		{
			EndChallenge(false);
			PlayerUI->ChallengeEnded(ChallengeType, false);
		}
		bUnderProgress = false;
		bCompleted = true;
	}
}

void ACoinStop::TempoHasPassed()
{
	if (!IsUndergoing() || bCompleted)
	{
		return;
	}

	TemposWithoutMoving++;
	if (TemposWithoutMoving >= TemposInStop)
	{
		bUnderProgress = false;
		EndChallenge(true);
		PlayerUI->ChallengeEnded(ChallengeType, true);
		bCompleted = true;
	}
}


