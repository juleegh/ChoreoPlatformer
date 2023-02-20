// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SongTempoComponent.generated.h"


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
	void CreateAudioImporter();
	UPROPERTY()
	FTimerHandle InTempoDelay;
	UPROPERTY()
	FTimerHandle NotInTempoDelay;
	UPROPERTY()
	float AcceptancePercentage = 0.6f;
	UPROPERTY()
	float SongDelay = 0.4688f;
	UPROPERTY()
	bool InTempo;
	UPROPERTY()
	float CurrentTime;
	UPROPERTY()
	class ADanceCharacter* character;

private:
	void SetInTempo();
	void SetNotInTempo();
	UPROPERTY()
	class URuntimeAudioImporterLibrary* RuntimeAudioImporter;
	UPROPERTY()
	class UAudioAnalysisToolsLibrary* AudioAnalyzer;

	UFUNCTION()
	void AudioDataReleased(const TArray<float>& AudioFrame);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOnTempo(float target = 1);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float TempoPercentage(float target = 1);
};
