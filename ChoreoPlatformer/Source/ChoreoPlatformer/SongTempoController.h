// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SongTempoController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(IsBlueprintBase = "true") )
class CHOREOPLATFORMER_API USongTempoController : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USongTempoController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void CreateAudioImporter();

private:
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
	bool IsOnTempo();
};
