// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "ChoreoPlayerController.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);

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
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapRangeBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapRangeEnd);
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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!SectionLevelManager)
	{
		return;
	}
	if(Section != SectionLevelManager->GetCurrentSection())
	{
		return;
	}
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
			ComponentGetters::GetDancerHealthComponent(GetWorld())->TakeHit();
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

void AWalkingEnemy::BeginPlay()
{
	Super::BeginPlay();
	//MoveTimeline->TimelineEnded.AddDynamic(this, &AWalkingEnemy::LookAtNextTarget);
}

void AWalkingEnemy::DoTempoAction()
{
	Super::DoTempoAction();
	if (PlayerController->IsPaused())
	{
		return;
	}

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
	if (CurrentTile.HitCell)
	{
		CurrentTile.HitCell->PromptDamage();
	}
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

void ARotatingEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ARotatingEnemy::DoTempoAction()
{
	Super::DoTempoAction();
	if (PlayerController->IsPaused())
	{
		return;
	}

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
	if (HitTile.HitCell)
	{
		HitTile.HitCell->PromptDamage();
	}
	StartedRotating(rotDirection);
	MarkNextTarget();
}


