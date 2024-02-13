// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CommonUserWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonButtonBase.h"
#include "DanceDefinitions.h"
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
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag Identifier;
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
	void PromptTempoResult(EMoveResult MoveResult, bool AnimationType);
};

UCLASS()
class CHOREOPLATFORMER_API UGameUI : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	void UpdateCountdown(int TemposLeft);
	UFUNCTION()
	void PromptTempoResult(EMoveResult MoveResult, bool AnimationType);

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
	UFUNCTION(BlueprintCallable)
	void ExitGame();
	bool IsScreenActive(const FGameplayTag& Screen);
	UFUNCTION(BlueprintCallable)
	void RemoveWidgetFromPile(const FGameplayTag& Identifier);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TMap<FGameplayTag, TSubclassOf<UCommonActivatableWidget>> WidgetClasses;
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UChoreoActivatableWidget*> GameWidgets;
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UChoreoActivatableWidget*> MenuWidgets;

	UFUNCTION(BlueprintImplementableEvent)
	void PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, const FGameplayTag WidgetType);
	UFUNCTION(BlueprintImplementableEvent)
	void PushGameWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, const FGameplayTag WidgetType);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveWidget(UChoreoActivatableWidget* WidgetClass);
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FGameplayTag> GetItems();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FClothingItemInfo& GetClothingItem(FGameplayTag ItemType);
};

UCLASS()
class CHOREOPLATFORMER_API ULevelCompleteCollectable : public UChoreoActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Collectable Button")
	void SetupnVisuals(FClothingItemInfo ClothingInfo)
	{
		OnSetupVisuals(ClothingInfo);
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "Collectable Button")
	void OnSetupVisuals(FClothingItemInfo ClothingInfo);
};

UCLASS()
class CHOREOPLATFORMER_API UCollectablesUI : public UChoreoActivatableWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* ItemsData;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ClothesBP;
	UPROPERTY()
	TMap<FGameplayTag, bool> OwnedItems;
	UPROPERTY()
	TMap<FGameplayTag, class AActor*> CurrentOutfit;
	UPROPERTY()
	class USkeletalMeshComponent* SkeletalMesh;
	
	void LoadOwnedItems();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TMap<FGameplayTag, bool> GetItems();

	UFUNCTION(BlueprintCallable)
	void PutOnItem(FGameplayTag ItemType);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FClothingItemInfo& GetClothingItem(FGameplayTag ItemType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasCollectedItem(FGameplayTag ItemType);
};

UCLASS()
class CHOREOPLATFORMER_API UCollectableButton : public UChoreoButtonBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Collectable Button")
	void SetupButtonVisuals(FClothingItemInfo ClothingInfo, bool IsAvailable)
	{
		OnSetupButtonVisuals(ClothingInfo, IsAvailable);
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "Collectable Button")
	void OnSetupButtonVisuals(FClothingItemInfo ClothingInfo, bool IsAvailable);
};

