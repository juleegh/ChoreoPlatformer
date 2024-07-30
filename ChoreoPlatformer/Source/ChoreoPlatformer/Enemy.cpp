// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "ChoreoPlayerController.h"
#include "SongTempoComponent.h"
#include "DancerHealthComponent.h"
#include "Components/SplineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"
#include "DanceCharacter.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	AttackIndicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Next Position"));
	AttackIndicator->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackIndicator->SetupAttachment(RootComponent);
	AttackIndicator->SetUsingAbsoluteLocation(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
	ColorTimeline = CreateDefaultSubobject<UColorTimelineComponent>("Color Timeline");
	ScaleTimeline = CreateDefaultSubobject<UScaleUpTimelineComponent>("Scale Timeline");
}

ARotatingEnemy::ARotatingEnemy()
{
	HitBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hit Box"));
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	AttackIndicator->SetWorldLocation(GetActorLocation());
	AttackIndicator->SetUsingAbsoluteLocation(true);
	MoveTimeline->Initialize();
	ColorTimeline->Initialize();
	ScaleTimeline->Initialize();
	ColorTimeline->AddMesh(GetMesh());
	ScaleTimeline->ScalingActor = AttackIndicator;
}

void AEnemy::SetupEnemy()
{
	SongTempo = ComponentGetters::GetSongTempoComponent(GetWorld());
	SectionLevelManager = ComponentGetters::GetSectionLevelManager(GetWorld());
	PlayerController = Cast<AChoreoPlayerController>(GetWorld()->GetFirstPlayerController());
	Section = FGameplayTag::EmptyTag;
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	if (CurrentTile.HitCell)
	{
		Section = CurrentTile.HitCell->GetSection();
	}
}

bool AEnemy::CurrentlyActive() const
{
	return Section != FGameplayTag::EmptyTag;
}

bool AEnemy::CanMove() const
{
	return !MoveTimeline->IsRunning() || MoveTimeline->ReachedHalfwayPoint();
}

void AEnemy::DoDamage(FVector DamageArea)
{
	FTileInfo CurrentDamage = UDanceUtilsFunctionLibrary::CheckPosition({ this }, DamageArea);
	if (CurrentDamage.HitCell)
	{
		CurrentDamage.HitCell->PromptDamage();
	}
	if (CurrentDamage.bHitPlayer)
	{
		ComponentGetters::GetDancerHealthComponent(GetWorld())->TakeHit();
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
	PatrolIndex = 0;
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

void ASplinedEnemy::MarkNextTarget()
{
	int TentativeIndex = PatrolIndex + 1;
	if (TentativeIndex == GetLastIndex())
	{
		TentativeIndex = 0;
	}
	FVector NextPosition = PatrolPoints[TentativeIndex];
	AttackIndicator->SetWorldLocation(NextPosition + FVector::UpVector * 5);

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(PatrolPoints[PatrolIndex], PatrolPoints[TentativeIndex]);
	FRotator Rotation = FRotator(0, 90 + LookAt.Yaw, -90);
	AttackIndicator->SetWorldRotation(Rotation);
}

void AWalkingEnemy::DoTempoAction()
{
	Super::DoTempoAction();
	if (PlayerController->IsPaused())
	{
		return;
	}
	DoDamage(PatrolPoints[PatrolIndex]);

	PatrolIndex++;
	if (PatrolIndex == GetLastIndex())
	{
		PatrolIndex = 0;
	}
	FVector Position = PatrolPoints[PatrolIndex];
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PatrolPoints[PatrolIndex]);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	SetActorRotation(Rotation);
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, Position);
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	float Speed = CurrentTile.TargetTempo * SongTempo->GetFrequency();
	MoveTimeline->MoveToPosition(NextTile.Position, Speed);
	ScaleTimeline->ScaleUp(FVector::Zero(), FVector::One(), Speed);
	ColorTimeline->Blink();
	StartedWalking();
	MarkNextTarget();
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

void ARotatingEnemy::DoTempoAction()
{
	Super::DoTempoAction();
	if (PlayerController->IsPaused())
	{
		return;
	}

	DoDamage(PatrolPoints[PatrolIndex]);
	DoDamage(GetActorLocation());
	PatrolIndex++;
	if (PatrolIndex == GetLastIndex())
	{
		PatrolIndex = 0;
	}
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PatrolPoints[PatrolIndex]);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, GetActorLocation());
	FTileInfo HitTile = UDanceUtilsFunctionLibrary::CheckPosition({ this }, PatrolPoints[PatrolIndex]);
	float Speed = CurrentTile.TargetTempo * SongTempo->GetFrequency();
	float rotDirection = (Rotation - GetActorRotation()).GetNormalized().Yaw;

	MoveTimeline->RotateToPosition(Rotation, Speed);
	ScaleTimeline->ScaleUp(FVector::Zero(), FVector::One(), Speed);
	ColorTimeline->Blink();

	StartedRotating(rotDirection);
	MarkNextTarget();
}

void AForwardEnemy::DoTempoAction()
{
	Super::DoTempoAction();
	if (PlayerController->IsPaused())
	{
		return;
	}
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, GetActorLocation());
	if (CurrentTile.bHitWater)
	{
		return;
	}
	DoDamage(GetActorLocation());

	FVector Position = GetNextTile(GetActorLocation());
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Position);
	FRotator Rotation = FRotator(0, LookAt.Yaw, 0);
	SetActorRotation(Rotation);

	
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, Position);

	float Speed = CurrentTile.TargetTempo * SongTempo->GetFrequency();
	MoveTimeline->MoveToPosition(NextTile.Position, Speed);
	ScaleTimeline->ScaleUp(FVector::Zero(), FVector::One(), Speed);
	ColorTimeline->Blink();
	StartedWalking();
	MarkNextTarget();
}

void AForwardEnemy::MarkNextTarget()
{
	FVector NextPosition = GetNextTile(GetNextTile(GetActorLocation()));
	AttackIndicator->SetWorldLocation(NextPosition + FVector::UpVector * -95);

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetNextTile(GetActorLocation()), NextPosition);
	FRotator Rotation = FRotator(0, 90 + LookAt.Yaw, -90);
	AttackIndicator->SetWorldRotation(Rotation);
}

FVector AForwardEnemy::GetNextTile(FVector Position)
{
	FVector Direction = GetActorForwardVector();
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, Position);
	if (!CurrentTile.HitCell)
	{
		return FVector::Zero();
	}
	if (CurrentTile.bForcesDirection)
	{
		Direction = CurrentTile.ForcedDirection;
	}
	Direction *= 100;

	FVector NextPosition = Position + Direction;
	FTileInfo NextTile = UDanceUtilsFunctionLibrary::CheckPosition({ this, ComponentGetters::GetDanceCharacter(GetWorld()) }, NextPosition);

	if (!NextTile.HitCell || NextTile.HitElement
		|| NextTile.HitCell->GetActorLocation().Z >= CurrentTile.HitCell->GetActorLocation().Z + 100
		|| NextTile.HitCell->GetActorLocation().Z <= CurrentTile.HitCell->GetActorLocation().Z - 100)
	{
		NextPosition = Position - Direction;
	}
	return NextPosition;
}