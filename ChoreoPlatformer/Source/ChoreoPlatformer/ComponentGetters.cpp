#include "ComponentGetters.h"
#include "Kismet/GameplayStatics.h"

namespace ComponentGetters
{
	ADanceCharacter* GetDanceCharacter(UWorld* WorldContextObject)
	{
		auto DanceController = WorldContextObject->GetFirstPlayerController();
		return Cast<ADanceCharacter>(DanceController->GetPawn());
	}

	UDancerHealthComponent* GetDancerHealthComponent(UWorld* WorldContextObject)
	{
		return Cast<UDancerHealthComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UDancerHealthComponent::StaticClass()));
	}

	UInventoryComponent* GetInventoryComponent(UWorld* WorldContextObject)
	{
		return Cast<UInventoryComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UInventoryComponent::StaticClass()));
	}

	USongTempoComponent* GetSongTempoComponent(UWorld* WorldContextObject)
	{
		return Cast<USongTempoComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(USongTempoComponent::StaticClass()));
	}

	ULevelEventsComponent* GetLevelEventsComponent(UWorld* WorldContextObject)
	{
		return Cast<ULevelEventsComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(ULevelEventsComponent::StaticClass()));
	}

	ULevelProgressComponent* GetLevelProgressComponent(UWorld* WorldContextObject)
	{
		return Cast<ULevelProgressComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(ULevelProgressComponent::StaticClass()));
	}

	ATilemapLevelManager* GetTilemapLevelManager(UWorld* WorldContextObject)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ATilemapLevelManager::StaticClass(), FoundManagers);

		for (auto Manager : FoundManagers)
		{
			return Cast<ATilemapLevelManager>(Manager);
		}

		return nullptr;
	}

	ASectionLevelManager* GetSectionLevelManager(UWorld* WorldContextObject)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ASectionLevelManager::StaticClass(), FoundManagers);

		for (auto Manager : FoundManagers)
		{
			return Cast<ASectionLevelManager>(Manager);
		}

		return nullptr;
	}
}

UDancerHealthComponent* UComponentGettersFunctionLibrary::GetDancerHealthComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetDancerHealthComponent(WorldContextActor->GetWorld());
}

USongTempoComponent* UComponentGettersFunctionLibrary::GetSongTempoComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetSongTempoComponent(WorldContextActor->GetWorld());
}