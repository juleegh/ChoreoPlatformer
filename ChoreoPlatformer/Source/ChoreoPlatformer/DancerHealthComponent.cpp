// Fill out your copyright notice in the Description page of Project Settings.


#include "DancerHealthComponent.h"
#include "ChoreoPlayerController.h"
#include "LevelProgressComponent.h"

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

void UDancerHealthComponent::CountStep(ETempoAccuracy result)
{
	if (!ShouldTakeDamage())
	{
		return;
	}

	Health += UDanceUtilsFunctionLibrary::GetHealthDelta(result);
	if (Cooldown > 0)
	{
		Cooldown--;
		if (Cooldown == 0)
		{
			CooldownChanged.Broadcast(false);
		}
	}
	Steps[result]++;
	if (Health > GetMaxHealth())
	{
		Health = GetMaxHealth();
	}
	if (Health <= 0)
	{
		Restart();
	}
	HealthChanged.Broadcast(GetCurrentHealth(), GetMaxHealth());
}

void UDancerHealthComponent::TakeHit(int Damage)
{
	if (!ShouldTakeDamage())
	{
		return;
	}
	if (Cooldown > 0)
	{
		Restart();
	}
	else
	{
		Cooldown = UDanceUtilsFunctionLibrary::GetDamageCooldown();
		CooldownChanged.Broadcast(true);
	}
}

void UDancerHealthComponent::Restart()
{
	Health = GetMaxHealth() / 2;
	Cooldown = 0;
	PlayerDied.Broadcast();
	CooldownChanged.Broadcast(false);
}
