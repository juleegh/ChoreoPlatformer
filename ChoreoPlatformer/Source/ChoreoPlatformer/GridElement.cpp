// Fill out your copyright notice in the Description page of Project Settings.


#include "GridElement.h"
#include "Components/BoxComponent.h"
#include "ChoreoPlayerController.h"
#include "ComponentGetters.h"

AGridElement::AGridElement()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxComponent;
}

void AGridElement::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGridElement::OnOverlapRangeBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGridElement::OnOverlapRangeEnd);
}

void AGridElement::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto character = Cast<ADanceCharacter>(OtherActor))
	{
		DanceCharacter = character;
		OnEnterRange();
	}
}

void AGridElement::OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (auto character = Cast<ADanceCharacter>(OtherActor))
	{
		DanceCharacter = nullptr;
		OnExitRange();
	}
}

void ACheckpoint::OnEnterRange()
{
	ComponentGetters::GetLevelProgressComponent(GetWorld())->ToggleCheckpoint(this);
	CheckpointReached();
}

void AMine::OnEnterRange()
{
	ComponentGetters::GetDancerHealthComponent(GetWorld())->TakeHit();
	Destroy();
}

void ATeleporter::OnEnterRange()
{
	if (OtherEnd)
	{
		DanceCharacter->SetActorLocation(OtherEnd->GetActorLocation());
	}
}

