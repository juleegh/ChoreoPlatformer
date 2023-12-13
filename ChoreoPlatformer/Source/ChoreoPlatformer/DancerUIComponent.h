// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CommonUserWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "DancerUIComponent.generated.h"

UCLASS()
class CHOREOPLATFORMER_API UChoreoButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		class UDancerUIComponent* GetDancerUIComponent();
	UFUNCTION(BlueprintCallable)
	void OnClicked();
}; 

UCLASS()
class CHOREOPLATFORMER_API UChoreoActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UDancerUIComponent* GetDancerUIComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class AChoreoPlayerController* GetController();
	UFUNCTION(BlueprintCallable)
	void SetSelected(UChoreoButtonBase* NewSelected);
protected:
	UPROPERTY(BlueprintReadWrite)
	class UChoreoButtonBase* SelectedButton;
};

UCLASS()
class CHOREOPLATFORMER_API UDancerStats : public UChoreoActivatableWidget
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
	static const FGameplayTag MainMenu;
	static const FGameplayTag CollectablesScreen;
	static const FGameplayTag Pause;
	static const FGameplayTag CalibrationScreen;
	static const FGameplayTag EndOfLevel;

	UFUNCTION()
	void UpdateCountdown(int TemposLeft);
	UFUNCTION()
	void PromptTempoResult(float Distance);

	UFUNCTION(BlueprintCallable)
	void LoadMenu();
	UFUNCTION(BlueprintCallable)
	void LoadGame();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CancelMenu();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ConfirmMenu();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangedCurrent(UChoreoButtonBase* SelectedButton);
	UFUNCTION(BlueprintCallable)
	void GoToMenuScreen(const FGameplayTag MenuScreen);
	UFUNCTION(BlueprintCallable)
	void GoToGameScreen(const FGameplayTag GameScreen);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class AChoreoPlayerController* GetController();
	void TogglePause();
	UFUNCTION(BlueprintCallable)
	void ExitGame();
	void RemoveGameWidget(FGameplayTag WidgetTag);
	bool IsPaused() { return bIsPaused; }

protected:
	bool bIsPaused = false;
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TMap<FGameplayTag, TSubclassOf<UCommonActivatableWidget>> WidgetClasses;
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UCommonActivatableWidget*> GameWidgets;
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UCommonActivatableWidget*> MenuWidgets;

	UFUNCTION(BlueprintImplementableEvent)
	void PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, const FGameplayTag WidgetType);
	UFUNCTION(BlueprintImplementableEvent)
	void PushGameWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, const FGameplayTag WidgetType);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveMenuWidget(UCommonActivatableWidget* WidgetClass);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveGameWidget(UCommonActivatableWidget* WidgetClass);
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
	void Confirm(const FInputActionValue& Value);
	void Cancel(const FInputActionValue& Value);
protected:
	UPROPERTY()
	TSubclassOf<UCommonActivatableWidget> GameUIClass;
	UPROPERTY()
	UGameUI* GameUI;

	virtual void BeginPlay() override;
};

UCLASS()
class CHOREOPLATFORMER_API UCalibrationScreen : public UChoreoActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void CalibrationStarted();
	UFUNCTION(BlueprintImplementableEvent)
	void CalibrationEnded();
};

UCLASS()
class CHOREOPLATFORMER_API ULevelSelectionUI : public UChoreoActivatableWidget
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
class CHOREOPLATFORMER_API ULevelCompleteUI : public UChoreoActivatableWidget
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

