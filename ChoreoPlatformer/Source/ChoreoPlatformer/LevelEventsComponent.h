// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "EventsDataAsset.h"
#include "LevelEventsComponent.generated.h"

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API ULevelEventsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelEventsComponent();

protected:

	UPROPERTY()
	UEventsDataAsset* LevelEvents;

	UPROPERTY()
	TMap<TSubclassOf<UUserWidget>, UUserWidget*> Widgets;
	
	UPROPERTY()
	TMap<FGameplayTag, int> Countdowns;

	void HandleWidgetEvent(FGameplayTag TriggerTag);
	void HandleCountdownEvent(FGameplayTag TriggerTag);

public:
	void ActivateTrigger(FGameplayTag TriggerTag);
};

UCLASS()
class CHOREOPLATFORMER_API AEventTrigger : public AActor
{
	GENERATED_BODY()

public:
	AEventTrigger();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditInstanceOnly)
	FGameplayTag ActorTrigger;
	UFUNCTION()
	void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

