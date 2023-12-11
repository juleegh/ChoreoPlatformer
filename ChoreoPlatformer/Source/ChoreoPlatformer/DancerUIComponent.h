// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "DancerUIComponent.generated.h"

UCLASS()
class CHOREOPLATFORMER_API UDancerUI : public UUserWidget
{
    GENERATED_BODY()
public:
    class USongTempoComponent* TempoComponent;
    UFUNCTION(BlueprintCallable, BlueprintPure)
    class USongTempoComponent* GetTempoComponent();
    UFUNCTION(BlueprintImplementableEvent)
    void InitializeScreen();
}; 

UCLASS()
class CHOREOPLATFORMER_API UDancerStats : public UDancerUI
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateCountdown(int TemposLeft);
    UFUNCTION(BlueprintImplementableEvent)
    void PromptTempoResult(float Distance);
};

UCLASS()
class CHOREOPLATFORMER_API UCalibrationScreen : public UDancerUI
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent)
    void CalibrationEnded();
};

UCLASS()
class CHOREOPLATFORMER_API UDancerUIComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDancerUIComponent();
    UFUNCTION()
    void UpdateCountdown(int TemposLeft);
    UFUNCTION()
    void PromptTempoResult(float Distance);

protected:
    UPROPERTY()
    TSubclassOf<UUserWidget> StatsClass;
    UPROPERTY()
    TSubclassOf<UUserWidget> CalibrationClass;
    UPROPERTY()
    UDancerStats* DancerStats;
    UPROPERTY()
    UCalibrationScreen* CalibrationScreen;
    virtual void BeginPlay() override;
};

UCLASS()
class CHOREOPLATFORMER_API ULevelCompleteUI : public UUserWidget
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag LevelEndTrigger;
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int GetStepsByAccuracy(ETempoAccuracy Accuracy);
    UFUNCTION(BlueprintCallable)
    void GoToNextSection();
};

