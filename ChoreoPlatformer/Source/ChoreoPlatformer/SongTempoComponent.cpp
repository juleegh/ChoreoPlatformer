// Fill out your copyright notice in the Description page of Project Settings.


#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RuntimeAudioImporterLibrary.h"
#include "Misc/Paths.h"
#include "AudioAnalysisToolsLibrary.h"

USongTempoComponent::USongTempoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USongTempoComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateAudioImporter();
}

void USongTempoComponent::CreateAudioImporter()
{
	RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	AudioAnalyzer = UAudioAnalysisToolsLibrary::CreateAudioAnalysisTools();

	if (!IsValid(RuntimeAudioImporter))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create audio importer"));
		return;
	}

	RuntimeAudioImporter->OnProgressNative.AddWeakLambda(this, [](int32 Percentage)
		{
			UE_LOG(LogTemp, Log, TEXT("Audio importing percentage: %d"), Percentage);
		});

	RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this](URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ETranscodingStatus Status)
		{
			if (Status == ETranscodingStatus::SuccessfulImport)
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully imported audio with sound wave %s"), *ImportedSoundWave->GetName());
				ImportedSoundWave->OnGeneratePCMData.AddDynamic(this, &USongTempoComponent::AudioDataReleased);
				ImportedSoundWave->SetLooping(true);
				UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}

			RuntimeAudioImporter = nullptr;
		});

	auto dir = FPaths::GameDevelopersDir() + "Sounds/Song.wav";
	RuntimeAudioImporter->ImportAudioFromFile(dir, EAudioFormat::Auto);
}

void USongTempoComponent::AudioDataReleased(const TArray<float>& AudioFrame)
{
	AudioAnalyzer->ProcessAudioFrame(AudioFrame);
}


bool USongTempoComponent::IsOnTempo()
{
	return AudioAnalyzer->IsBeat(7) || AudioAnalyzer->IsHiHat();
}

void USongTempoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

