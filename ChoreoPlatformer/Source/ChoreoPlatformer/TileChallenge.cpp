// Fill out your copyright notice in the Description page of Project Settings.


#include "TileChallenge.h"
#include "ChoreoPlayerController.h"
#include "DancerUIComponent.h"
#include "Components/SplineComponent.h"
#include "DanceCharacter.h"
#include "DanceUtilsFunctionLibrary.h"

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
	Player = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	Player->PlayerNewPosition.AddDynamic(this, &ATileChallenge::PlayerChangedPosition);
	auto PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerUI = PlayerController->GetDancerUIComponent();
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
	if (!bUnderProgress && GetPlayerCurrentIndex() == 0)
	{
		bUnderProgress = true;
		StartChallenge();
		ChallengeStarted();
		PlayerUI->ChallengeStarted(ChallengeType);
	}
	else if (bUnderProgress && GetPlayerCurrentIndex() == InterestPoints->GetNumberOfSplinePoints() - 1)
	{
		ChallengeEnded(true);
		PlayerUI->ChallengeEnded(ChallengeType, true);
		bUnderProgress = false;
	}
}

bool ATileChallenge::IsUndergoing()
{
	return bUnderProgress;
}

void AHalfCoin::StartChallenge()
{
	TemposRemaining = TemposBetweenHalfs;
}

void AHalfCoin::PlayerChangedPosition()
{
	Super::PlayerChangedPosition();

	if (bUnderProgress)
	{
		TemposRemaining--;
		ChallengeUpdated();
	}
	if (bUnderProgress && TemposRemaining == 0)
	{
		bUnderProgress = false;
		ChallengeEnded(false);
		PlayerUI->ChallengeEnded(ChallengeType, false);
	}
}

void ACoinTrail::StartChallenge()
{
	CurrentCoin = -1;
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
			ChallengeEnded(false);
			PlayerUI->ChallengeEnded(ChallengeType, false);
		}
	}
}


