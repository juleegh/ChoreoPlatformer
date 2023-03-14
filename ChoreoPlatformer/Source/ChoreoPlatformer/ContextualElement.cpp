#include "ContextualElement.h"
#include "Components/BoxComponent.h"
#include "LevelProgressComponent.h"
#include "ChoreoPlayerController.h"
#include "DancerHealthComponent.h"
#include "DanceCharacter.h"

AContextualElement::AContextualElement()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxComponent;
}

void AContextualElement::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABrickWall::TriggerInteraction()
{
	HitsLeft--;
	if (HitsLeft <= 0)
	{
		Destroy();
	}
}
