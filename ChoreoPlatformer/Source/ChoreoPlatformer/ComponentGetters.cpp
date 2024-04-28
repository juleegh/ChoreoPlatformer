#include "ComponentGetters.h"
#include "GridCell.h"
#include "DanceDefinitions.h"
#include "TilemapLevelManager.h"
#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "DancerHealthComponent.h"
#include "LevelProgressComponent.h"
#include "SongTempoComponent.h"
#include "DancerUIComponent.h"
#include "DanceAudioManager.h"
#include "CalibrationComponent.h"
#include "InventoryComponent.h"
#include "GameCameraComponent.h"
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

	UCalibrationComponent* GetCalibrationComponent(UWorld* WorldContextObject)
	{
		return Cast<UCalibrationComponent>(WorldContextObject->GetFirstPlayerController()->GetComponentByClass(UCalibrationComponent::StaticClass()));
	}

	UGameCameraComponent* GetGameCameraComponent(UWorld* WorldContextObject)
	{
		return Cast<UGameCameraComponent>(GetDanceCharacter(WorldContextObject)->GetComponentByClass(UGameCameraComponent::StaticClass()));
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
	
	AEndlessLevelManager* GetEndlessLevelManager(UWorld* WorldContextObject)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AEndlessLevelManager::StaticClass(), FoundManagers);

		for (auto Manager : FoundManagers)
		{
			return Cast<AEndlessLevelManager>(Manager);
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

	ASectionStart* GetSectionStart(UWorld* WorldContextObject, const FGameplayTag& SectionIdentifier)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ASectionStart::StaticClass(), FoundActors);

		for (auto Section : FoundActors)
		{
			if (auto LevelSection = Cast<ASectionStart>(Section))
			{
				if (LevelSection->GetSectionIdentifier() != SectionIdentifier)
				{
					continue;
				}

				return LevelSection;
			}
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
	return ComponentGetters::GetController(WorldContextActor->GetWorld());
}

ULevelEventsComponent* UComponentGettersFunctionLibrary::GetLevelEventsComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetLevelEventsComponent(WorldContextActor->GetWorld());
}
