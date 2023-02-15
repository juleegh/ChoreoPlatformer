// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "DancerUIComponent.generated.h"

USTRUCT(BlueprintType)
struct FStateUI
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    int CurrentHealth;
    UPROPERTY(BlueprintReadOnly)
    int MaxHealth;
};

UCLASS()
class CHOREOPLATFORMER_API UDancerUI : public UUserWidget
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateUIState(FStateUI StateUI);
	
};

UCLASS()
class CHOREOPLATFORMER_API UDancerUIComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDancerUIComponent();
    UFUNCTION()
    void UpdateHealth(int Current, int Max);

protected:
    UPROPERTY()
    TSubclassOf<UUserWidget> DancerClass;
    UPROPERTY()
    UDancerUI* DancerUI;
    UPROPERTY()
    FStateUI StateUI;
    virtual void BeginPlay() override;
};

