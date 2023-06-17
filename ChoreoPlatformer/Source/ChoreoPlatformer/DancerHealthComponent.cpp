// Fill out your copyright notice in the Description page of Project Settings.


#include "DancerHealthComponent.h"
#include "ChoreoPlayerController.h"
#include "LevelProgressComponent.h"
#include "InventoryComponent.h"
#include "DanceUtilsFunctionLibrary.h"

UDancerHealthComponent::UDancerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDancerHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Accuracy = GetMaxAccuracy() / 2;
	Steps = TMap<ETempoAccuracy, int>();
	Steps.Add(ETempoAccuracy::Bad, 0);
	Steps.Add(ETempoAccuracy::Great, 0);
	Steps.Add(ETempoAccuracy::Perfect, 0);
}

bool UDancerHealthComponent::ShouldTakeDamage()
{
	if (auto Controller = Cast<AChoreoPlayerController>(GetOwner()))
	{
		return Controller->ShouldTakeDamage();
	}
	return true;
}

float UDancerHealthComponent::GetTotalSteps()
{
	return Steps[ETempoAccuracy::Perfect] + Steps[ETempoAccuracy::Great] + Steps[ETempoAccuracy::Bad];
}


void UDancerHealthComponent::CountStep(ETempoAccuracy result)
{
	Steps[result]++;
	if (!ShouldTakeDamage())
	{
		return;
	}

	Accuracy += UDanceUtilsFunctionLibrary::GetHealthDelta(result);
	if (Accuracy > GetMaxAccuracy())
	{
		Accuracy = GetMaxAccuracy();
	}
	if (Accuracy < 0)
	{
		Accuracy = 0;
	}
	AccuracyChanged.Broadcast(GetCurrentAccuracy(), GetMaxAccuracy());
}

void UDancerHealthComponent::TakeHit(int Damage)
{
	if (!ShouldTakeDamage())
	{
		return;
	}
	if (!UDanceUtilsFunctionLibrary::GetInventoryComponent(GetOwner())->LoseHealthItem())
	{
		Restart();
		HealthChanged.Broadcast(false, false);
	}
	else
	{
		HealthChanged.Broadcast(false, UDanceUtilsFunctionLibrary::GetInventoryComponent(GetOwner())->HasHealthItem());
	}
}

void UDancerHealthComponent::Restart()
{
	Accuracy = GetMaxAccuracy() / 2;
	PlayerDied.Broadcast();
}
