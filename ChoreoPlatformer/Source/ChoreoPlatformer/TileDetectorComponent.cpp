#include "TileDetectorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "ContextualElement.h"
#include "DanceUtilsFunctionLibrary.h"

UTileDetectorComponent::UTileDetectorComponent()
{

}

void UTileDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetOwner();
}

void UTileDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FDetectedInfo UTileDetectorComponent::CheckPosition(FVector Start)
{
	FHitResult OutHit;
	FDetectedInfo DetectedInfo;

	Start.Z -= 50.f;

	FVector DownVector = -FVector::ZAxisVector;
	FVector End = ((DownVector * 100) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerPawn);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetActor()->GetName()));
		if(auto actor = Cast<AGridCell>(OutHit.GetActor()))
		{
			DetectedInfo.TileType = actor->GetTileType();
			DetectedInfo.TargetTempo = UDanceUtilsFunctionLibrary::GetTargetTempo(actor->GetTileType());
			DetectedInfo.bForcesDirection = actor->ForcesPlayerPosition();
			DetectedInfo.ForcedDirection = actor->ForcedDirection();
		}
	}

	Start.Z += 50.f;

	DetectedInfo.bHitElement = false;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		if (auto element = Cast<AContextualElement>(OutHit.GetActor()))
		{
			DetectedInfo.bHitElement = true;
			DetectedInfo.HitElement = element;
		}
	}

	return DetectedInfo;
}