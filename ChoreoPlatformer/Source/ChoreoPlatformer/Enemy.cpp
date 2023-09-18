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
#include "DanceUtilsFunctionLibrary.h"

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
	MoveTimeline = CreateDefaultSubobject<UTimelineCreatorComponent>("Move Timeline");
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapRangeBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapRangeEnd);
	auto PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	SongTempo = PlayerController->GetSongTempoComponent();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	float Result = SongTempo->TempoResult(CurrentTile.TargetTempo);

	if (hasDoneTempoAction)
	{
		if (Result > UDanceUtilsFunctionLibrary::GetPerfectAcceptanceRate())
		{
			hasDoneTempoAction = false;
		}
	}
	else
	{
		if (Result <= UDanceUtilsFunctionLibrary::GetPerfectAcceptanceRate())
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
		PlayerCharacter = character;
		if (!MoveTimeline->IsRunning())
		{
			character->GetChoreoController()->GetDancerHealthComponent()->TakeHit();
			PlayerCharacter = nullptr;
		}
	}
}

void AEnemy::OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (auto character = Cast<ADanceCharacter>(OtherActor))
	{
		PlayerCharacter = nullptr;
	}
}


void AEnemy::DoTempoAction()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetChoreoController()->GetDancerHealthComponent()->TakeHit();
	}
}

ASplinedEnemy::ASplinedEnemy()
{
	PatrolRoad = CreateDefaultSubobject<USplineComponent>(TEXT("Patrol Road"));
	PatrolRoad->SetupAttachment(RootComponent);
}

void ASplinedEnemy::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < GetLastIndex(); i++)
	{
		PatrolPoints.Add(GetWorldLocationByIndex(i));
	}
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

void AWalkingEnemy::BeginPlay()
{
	Super::BeginPlay();
	MoveTimeline->Initialize();
	//MoveTimeline->TimelineEnded.AddDynamic(this, &AWalkingEnemy::LookAtNextTarget);
}

void AWalkingEnemy::DoTempoAction()
{
	Super::DoTempoAction();

	PatrolIndex++;
	if (PatrolIndex == GetLastIndex())
	{
		PatrolIndex = 0;
	}
	FVector Position = PatrolPoints[PatrolIndex];
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PatrolPoints[PatrolIndex]);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	SetActorRotation(Rotation);
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, UDanceUtilsFunctionLibrary::GetDanceCharacter(this) }, Position);
	float Speed = NextTile.TargetTempo * SongTempo->GetFrequency() * 0.95f;
	MoveTimeline->MoveToPosition(NextTile.Position, 0.25f);
	StartedWalking();
}

void AWalkingEnemy::LookAtNextTarget()
{
	int LookIndex = PatrolIndex + 1;
	if (LookIndex == GetLastIndex())
	{
		LookIndex = 0;
	}
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PatrolPoints[LookIndex]);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	MoveTimeline->RotateToPosition(Rotation, 0.25f);
}

void ARotatingEnemy::BeginPlay()
{
	Super::BeginPlay();
	MoveTimeline->Initialize();
}

void ARotatingEnemy::DoTempoAction()
{
	Super::DoTempoAction();

	PatrolIndex++;
	if (PatrolIndex == GetLastIndex())
	{
		PatrolIndex = 0;
	}
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PatrolPoints[PatrolIndex]);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	float Speed = CurrentTile.TargetTempo * SongTempo->GetFrequency() * 0.95f;
	MoveTimeline->RotateToPosition(Rotation, Speed);
	StartedRotating();
}


