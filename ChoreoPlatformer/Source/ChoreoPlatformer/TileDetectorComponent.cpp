#include "TileDetectorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

UTileDetectorComponent::UTileDetectorComponent()
{

}

void UTileDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void UTileDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AGridCell* UTileDetectorComponent::CheckTile(FVector Start)
{
	FHitResult OutHit;

	Start.Z -= 50.f;

	FVector DownVector = -FVector::ZAxisVector;
	FVector End = ((DownVector * 100) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerPawn);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetActor()->GetName()));
		auto actor = Cast<AGridCell>(OutHit.GetActor());
		if (actor != NULL)
		{
			return actor;
		}
	}

	return nullptr;
}
