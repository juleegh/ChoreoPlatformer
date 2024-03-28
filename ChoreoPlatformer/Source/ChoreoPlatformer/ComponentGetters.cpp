#include "ComponentGetters.h"
#include "Kismet/GameplayStatics.h"

namespace ComponentGetters
{
	AChoreoPlayerController* GetController(UWorld* WorldContextObject)
	{
		auto DanceController = WorldContextObject->GetFirstPlayerController();
		return Cast<AChoreoPlayerController>(DanceController);
	}

	ADanceCharacter* GetDanceCharacter(UWorld* WorldContextObject)
	{
		auto DanceController = WorldContextObject->GetFirstPlayerController();
		return Cast<ADanceCharacter>(DanceController->GetPawn());
	}

	ADanceAudioManager* GetDanceAudioManager(UWorld* WorldContextObject)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ADanceAudioManager::StaticClass(), FoundManagers);

		for (auto Manager : FoundManagers)
		{
			return Cast<ADanceAudioManager>(Manager);
		}

		return nullptr;
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

	UDancerUIComponent* GetDancerUIComponent(UWorld* WorldContextObject)
	{
		return Cast<UDancerUIComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UDancerUIComponent::StaticClass()));
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

UInventoryComponent* UComponentGettersFunctionLibrary::GetInventoryComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetInventoryComponent(WorldContextActor->GetWorld());
}

ASectionLevelManager* UComponentGettersFunctionLibrary::GetSectionLevelManager(AActor* WorldContextActor)
{
	return ComponentGetters::GetSectionLevelManager(WorldContextActor->GetWorld());
}

AChoreoPlayerController* UComponentGettersFunctionLibrary::GetChoreoPlayerController(AActor* WorldContextActor)
{
	auto DanceController = WorldContextActor->GetWorld()->GetFirstPlayerController();
	return Cast<AChoreoPlayerController>(DanceController);
}

ULevelEventsComponent* UComponentGettersFunctionLibrary::GetLevelEventsComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetLevelEventsComponent(WorldContextActor->GetWorld());
}
