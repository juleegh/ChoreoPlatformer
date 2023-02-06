#include "TileDetectorComponent.h"
#include "Kismet/GameplayStatics.h"

UTileDetectorComponent::UTileDetectorComponent()
{

}

void UTileDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTileDetectorComponent::CheckTile()
{
	FHitResult OutHit;
	FVector Start = GetOwner()->GetActorLocation();

	Start.Z -= 100.f;

	FVector DownVector = -GetOwner()->GetActorUpVector();
	FVector End = ((DownVector * 500.f) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);

	if (GetOwner()->ActorLineTraceSingle(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetActor()->GetName()));
	}
}
