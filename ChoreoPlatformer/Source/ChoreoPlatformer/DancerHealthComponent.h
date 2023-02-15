// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DancerHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDied);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHOREOPLATFORMER_API UDancerHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDancerHealthComponent();

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	int CurrentHealth;
	UPROPERTY()
	int MaxHealth = 3;
public:	
	UPROPERTY()
	FPlayerDied PlayerDied;
	void TakeHit(int Damage = 1);
};
