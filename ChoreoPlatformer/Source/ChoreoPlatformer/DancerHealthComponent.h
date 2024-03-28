// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DanceUtilsFunctionLibrary.h"
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
	bool ShouldTakeDamage();
	UPROPERTY()
	TMap<ETempoAccuracy, int> Steps;
	UPROPERTY()
	float Accuracy;
	float GetTotalSteps();
public:	
	UPROPERTY(BlueprintAssignable)
	FPlayerDied PlayerDied;
	void Restart();
	void TakeHit(int Damage = 1);
	void CountStep(ETempoAccuracy result);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentAccuracy() const { return Accuracy; }
	float GetMaxAccuracy() const { return 1; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStepsByAccuracy(ETempoAccuracy TempoAccuracy) { return (Steps[TempoAccuracy] / GetTotalSteps() * 100); }
};
