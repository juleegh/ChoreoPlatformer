// Fill out your copyright notice in the Description page of Project Settings.


#include "DancerHealthComponent.h"
#include "ChoreoPlayerController.h"
#include "DanceUtilsFunctionLibrary.h"
#include "ComponentGetters.h"

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
	Accuracy += UDanceUtilsFunctionLibrary::GetHealthDelta(result);
	Accuracy = FMath::Clamp(Accuracy, 0, GetMaxAccuracy());
	AccuracyChanged.Broadcast(GetCurrentAccuracy(), GetMaxAccuracy());
}

void UDancerHealthComponent::TakeHit(int Damage)
{
	if (!ShouldTakeDamage())
	{
		return;
	}
	if (!ComponentGetters::GetInventoryComponent(GetWorld())->LoseHealthItem())
	{
		Restart();
		HealthChanged.Broadcast(false, false, true);
	}
	else
	{
		HealthChanged.Broadcast(false, ComponentGetters::GetInventoryComponent(GetWorld())->HasHealthItem(), false);
	}
}

void UDancerHealthComponent::Restart()
{
	Accuracy = GetMaxAccuracy() / 2;
	PlayerDied.Broadcast();
}