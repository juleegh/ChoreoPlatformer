// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DancerHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChanged, int, Health, int, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDied);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHOREOPLATFORMER_API UDancerHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDancerHealthComponent();

protected:
	virtual void BeginPlay() override;
	bool ShouldTakeDamage();
	UPROPERTY()
	int CurrentHealth;
	UPROPERTY()
	int MaxHealth = 3;
public:	
	UPROPERTY()
	FHealthChanged HealthChanged;
	UPROPERTY()
	FPlayerDied PlayerDied;
	void TakeHit(int Damage = 1);
	int GetCurrentHealth() const { return CurrentHealth; }
	int GetMaxHealth() const { return MaxHealth; }
};
