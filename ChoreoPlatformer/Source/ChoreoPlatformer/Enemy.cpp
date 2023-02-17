// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "ChoreoPlayerController.h"
#include "DanceCharacter.h"
#include "DancerHealthComponent.h"
#include "SongTempoComponent.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimelineCreatorComponent.h"
#include "Kismet/KismetMathLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapRangeBegin);
	auto PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	SongTempo = PlayerController->GetSongTempoComponent();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (hasDoneTempoAction)
	{
		if (!SongTempo->IsOnTempo())
		{
			hasDoneTempoAction = false;
		}
	}
	else
	{
		if (SongTempo->IsOnTempo())
		{
			hasDoneTempoAction = true;
			DoTempoAction();
		}
	}
}

void AEnemy::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto character = Cast<ADanceCharacter>(OtherActor))
	{
		character->GetChoreoController()->GetDancerHealthComponent()->TakeHit();
	}
}

void AEnemy::DoTempoAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Did action")));
}

ASplinedEnemy::ASplinedEnemy()
{
	PatrolRoad = CreateDefaultSubobject<USplineComponent>(TEXT("Patrol Road"));
	PatrolRoad->SetupAttachment(RootComponent);
}

FVector ASplinedEnemy::GetWorldLocationByIndex(int Index) const
{
	if (PatrolRoad && Index < GetLastIndex())
	{
		return PatrolRoad->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
	}
	return GetActorLocation();
}

int ASplinedEnemy::GetLastIndex() const
{
	if (PatrolRoad)
	{
		return PatrolRoad->GetNumberOfSplinePoints();
	}

	return 0;
}

AWalkingEnemy::AWalkingEnemy()
{
	MoveTimeline = CreateDefaultSubobject<UMoveTimeline>("Move Timeline");
}

void AWalkingEnemy::DoTempoAction()
{
	PatrolIndex++;
	if (PatrolIndex >= GetLastIndex())
	{
		PatrolIndex = 0;
	}
	FVector Position = GetWorldLocationByIndex(PatrolIndex);
	Position.Z = GetActorLocation().Z;
	MoveTimeline->MoveToPosition(Position);
}

ARotatingEnemy::ARotatingEnemy()
{
	RotateTimeline = CreateDefaultSubobject<URotateTimeline>("Rotate Timeline");
}

void ARotatingEnemy::DoTempoAction()
{
	PatrolIndex++;
	if (PatrolIndex >= GetLastIndex())
	{
		PatrolIndex = 0;
	}
	//FRotator LookAt = GetWorldLocationByIndex(PatrolIndex);
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetWorldLocationByIndex(PatrolIndex));
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	RotateTimeline->RotateToPosition(Rotation);
}


