// Fill out your copyright notice in the Description page of Project Settings.


#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RuntimeAudioImporterLibrary.h"
#include "Misc/Paths.h"
#include "AudioAnalysisToolsLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DanceCharacter.h"

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
				character = Cast<ADanceCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
				UE_LOG(LogTemp, Warning, TEXT("Successfully imported audio with sound wave %s"), *ImportedSoundWave->GetName());
				ImportedSoundWave->OnGeneratePCMData.AddDynamic(this, &USongTempoComponent::AudioDataReleased);
				ImportedSoundWave->SetLooping(true);
				UGameplayStatics::PlaySound2D(GetWorld(), ImportedSoundWave);
				GetWorld()->GetTimerManager().SetTimer(NotInTempoDelay, this, &USongTempoComponent::SetInTempo, SongDelay * (1 - AcceptancePercentage + AcceptancePercentage / 2));
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

void USongTempoComponent::SetInTempo()
{
	InTempo = true;
	GetWorld()->GetTimerManager().SetTimer(InTempoDelay, this, &USongTempoComponent::SetNotInTempo, SongDelay * AcceptancePercentage);
}

void USongTempoComponent::SetNotInTempo()
{
	InTempo = false;
	GetWorld()->GetTimerManager().SetTimer(NotInTempoDelay, this, &USongTempoComponent::SetInTempo, SongDelay * (1 - AcceptancePercentage));
}


bool USongTempoComponent::IsOnTempo(float target)
{
	float Residue = TempoPercentage(target);
	return Residue < AcceptancePercentage * 0.5f || Residue >= (target - AcceptancePercentage * 0.5f);
}

float USongTempoComponent::TempoPercentage(float target)
{
	float Whole = FMath::FloorToInt(CurrentTime / SongDelay * target);
	float Residue = (CurrentTime / SongDelay) - Whole;
	return 1 - Residue;
}

void USongTempoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurrentTime += DeltaTime;
}

