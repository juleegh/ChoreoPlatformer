// Fill out your copyright notice in the Description page of Project Settings.


#include "SongTempoController.h"
#include "Kismet/GameplayStatics.h"
#include "RuntimeAudioImporterLibrary.h"
#include "Misc/Paths.h"
#include "AudioAnalysisToolsLibrary.h"

USongTempoController::USongTempoController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USongTempoController::BeginPlay()
{
	Super::BeginPlay();
	CreateAudioImporter();
}

void USongTempoController::CreateAudioImporter()
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
				// Here you can handle ImportedSoundWave playback, like "UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);"
				ImportedSoundWave->OnGeneratePCMData.AddDynamic(this, &USongTempoController::AudioDataReleased);
				ImportedSoundWave->SetLooping(true);
				UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}

			RuntimeAudioImporter = nullptr;
		});

	//C:/Users/Julia/Documents
	auto dir = FPaths::GameDevelopersDir() + "Sounds/Song.wav";
	RuntimeAudioImporter->ImportAudioFromFile(dir, EAudioFormat::Auto);
}

void USongTempoController::AudioDataReleased(const TArray<float>& AudioFrame)
{
	AudioAnalyzer->ProcessAudioFrame(AudioFrame);
}


bool USongTempoController::IsOnTempo()
{
	return AudioAnalyzer->IsBeat(7);
}

void USongTempoController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

