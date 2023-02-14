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

void UDancerHealthComponent::TakeHit(int Damage)
{
	CurrentHealth--;
	if (CurrentHealth == 0)
	{
		auto controller = Cast<AChoreoPlayerController>(GetOwner());
		controller->GetLevelProgressComponent()->LoadCheckpoint();
		CurrentHealth = MaxHealth;
	}
}