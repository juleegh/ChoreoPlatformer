// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SongTempoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewTempoStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTempoCountdown, int, Tempos);

UCLASS()
class CHOREOPLATFORMER_API USongTempoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USongTempoComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY()
	float SongDelay = 0.4688f;
	UPROPERTY()
	bool InTempo;
	UPROPERTY()
	float CurrentTime;
	UPROPERTY()
	int CurrentPauseTempos;
	UPROPERTY()
	class ADanceCharacter* character;
	float GetAcceptancePercentage();
public:	
	UPROPERTY()
	FNewTempoStarted NewTempoStarted;
	UPROPERTY()
	FTempoCountdown TempoCountdown;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOnTempo(float target = 1);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float TempoResult(float target);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float TempoPercentage();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool TempoPercentageIsAcceptable(float target = 1);
	void AddPauseTempos(int);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetRemainingPauseTempos() { return CurrentPauseTempos; }
};
