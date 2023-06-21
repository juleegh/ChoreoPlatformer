// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "TileChallenge.h"
#include "DancerUIComponent.generated.h"

USTRUCT(BlueprintType)
struct FStateUI
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    float CurrentHealth;
    UPROPERTY(BlueprintReadOnly)
    float MaxHealth;
};

UCLASS()
class CHOREOPLATFORMER_API UDancerUI : public UUserWidget
{
	GENERATED_BODY()
public:
    class USongTempoComponent* TempoComponent;
    UFUNCTION(BlueprintCallable, BlueprintPure)
    class USongTempoComponent* GetTempoComponent();
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateFruitCounter(int Fruit, bool DeltaType);
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateCountdown(int TemposLeft);
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateUIState(FStateUI StateUI);
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
    void UpdateAccuracy(float Current, float Max);
    UFUNCTION()
    void UpdateHealth(bool bPositiveDelta, bool bHasItems, bool bDied);
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
    UPROPERTY()
    FStateUI StateUI;
    virtual void BeginPlay() override;
};

UCLASS()
class CHOREOPLATFORMER_API ULevelCompleteUI : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int GetStepsByAccuracy(ETempoAccuracy Accuracy);
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetChallengeResultByType(EChallengeType ChallengeType);
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FText GetFruitCount();
};

