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
	Health = GetMaxHealth() / 2;
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

	Health += UDanceUtilsFunctionLibrary::GetHealthDelta(result);
	if (Health > GetMaxHealth())
	{
		Health = GetMaxHealth();
	}
	if (Health < 0)
	{
		Health = 0;
	}
	HealthChanged.Broadcast(GetCurrentHealth(), GetMaxHealth());
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
	}
	else
	{
		CooldownChanged.Broadcast(true);
	}
}

void UDancerHealthComponent::Restart()
{
	Health = GetMaxHealth() / 2;
	PlayerDied.Broadcast();
}
