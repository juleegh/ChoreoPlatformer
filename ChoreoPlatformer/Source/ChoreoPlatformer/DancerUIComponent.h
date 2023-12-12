// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CommonUserWidget.h"
#include "CommonActivatableWidget.h"
#include "DancerUIComponent.generated.h"

UCLASS()
class CHOREOPLATFORMER_API UDancerStats : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateCountdown(int TemposLeft);
	UFUNCTION(BlueprintImplementableEvent)
		void PromptTempoResult(float Distance);
};

UCLASS()
class CHOREOPLATFORMER_API UGameUI : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	static const FGameplayTag LevelSelection;
	static const FGameplayTag GameStats;

	UFUNCTION()
		void UpdateCountdown(int TemposLeft);
	UFUNCTION()
		void PromptTempoResult(float Distance);

	UFUNCTION(BlueprintCallable)
	void LoadMenu();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UCommonActivatableWidget> StatsClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UCommonActivatableWidget> CalibrationClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UCommonActivatableWidget> LevelSelectionClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UCommonActivatableWidget> LevelCompleteClass;
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UCommonActivatableWidget*> GameWidgets;
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UCommonActivatableWidget*> MenuWidgets;

	UFUNCTION(BlueprintImplementableEvent)
	void PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, const FGameplayTag WidgetType);
	UFUNCTION(BlueprintImplementableEvent)
	void PushGameWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, const FGameplayTag WidgetType);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveGameWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	UFUNCTION(BlueprintImplementableEvent)
	void ClearMenuWidgets();
	UFUNCTION(BlueprintImplementableEvent)
	void ClearGameWidgets();

};

UCLASS()
class CHOREOPLATFORMER_API UDancerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDancerUIComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGameUI* GetGameUI() { return GameUI; }

protected:
	UPROPERTY()
	TSubclassOf<UCommonActivatableWidget> GameUIClass;
	UPROPERTY()
	UGameUI* GameUI;

	virtual void BeginPlay() override;
};

UCLASS()
class CHOREOPLATFORMER_API UCalibrationScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void CalibrationStarted();
	UFUNCTION(BlueprintImplementableEvent)
	void CalibrationEnded();
};

UCLASS()
class CHOREOPLATFORMER_API ULevelSelectionUI : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ChangedLevelSelected(int index);
	UFUNCTION(BlueprintCallable)
	void LoadSelected();
protected:
	int LevelIndex = 1;
}; 

UCLASS()
class CHOREOPLATFORMER_API ULevelCompleteUI : public UCommonActivatableWidget
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

