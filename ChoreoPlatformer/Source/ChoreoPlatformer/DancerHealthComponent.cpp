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
	CurrentHealth = MaxHealth;
}

bool UDancerHealthComponent::ShouldTakeDamage()
{
	if (auto Controller = Cast<AChoreoPlayerController>(GetOwner()))
	{
		return Controller->ShouldTakeDamage();
	}
	return true;
}

void UDancerHealthComponent::TakeHit(int Damage)
{
	if (!ShouldTakeDamage())
	{
		return;
	}
	CurrentHealth--;
	if (CurrentHealth == 0)
	{
		PlayerDied.Broadcast();
		CurrentHealth = MaxHealth;
	}
	HealthChanged.Broadcast(CurrentHealth, MaxHealth);
}
