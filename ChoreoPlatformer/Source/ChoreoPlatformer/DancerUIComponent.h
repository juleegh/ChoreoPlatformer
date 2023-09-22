// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "TileChallenge.h"
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
    void UpdateCountdown(int TemposLeft);
    UFUNCTION(BlueprintImplementableEvent)
    void PromptTempoResult(float Distance);
    UFUNCTION(BlueprintImplementableEvent)
    void InitializeEvents();
};

UCLASS()
class CHOREOPLATFORMER_API UChallengeUI : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent)
    void ChallengeStarted(EChallengeType ChallengeType);
    UFUNCTION(BlueprintImplementableEvent)
    void ChallengeEnded(EChallengeType ChallengeType, bool Success);
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
    UFUNCTION()
    void ChallengeStarted(EChallengeType ChallengeType);
    UFUNCTION()
    void ChallengeEnded(EChallengeType ChallengeType, bool Success);

protected:
    UPROPERTY()
    TSubclassOf<UUserWidget> DancerClass;
    UPROPERTY()
    TSubclassOf<UUserWidget> ChallengeClass;
    UPROPERTY()
    UDancerUI* DancerUI;
    UPROPERTY()
    UChallengeUI* ChallengeUI;
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
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetChallengeResultByType(EChallengeType ChallengeType);
    UFUNCTION(BlueprintCallable)
    void GoToNextSection();
};

