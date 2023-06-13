// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ContextualElement.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AContextualElement : public AActor
{
	GENERATED_BODY()

public:
	AContextualElement();

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshState();
	virtual void TriggerInteraction() {}
	//virtual void Tick(float DeltaTime) override;
};

UCLASS()
class CHOREOPLATFORMER_API ABrickWall : public AContextualElement
{
	GENERATED_BODY()

public:
	ABrickWall() {}

protected:
	UPROPERTY()
	int HitsLeft = 2;
	void TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API ADoor : public AContextualElement
{
	GENERATED_BODY()

public:
	ADoor() {}

protected:
};

UCLASS()
class CHOREOPLATFORMER_API ALever : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly)
	TArray<ADoor*> ConnectedDoors;
public:
	ALever() {}
	void TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API AItem : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag ItemType;
	UPROPERTY()
	bool bFinished;
public:
	void TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API AItemObstacle : public AContextualElement
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag RequiredItem;
	UPROPERTY()
	bool bFinished;

	void RemoveObstacle();
	virtual void PostObstacleActions() {}
public:
	void TriggerInteraction() override;
};

UCLASS()
class CHOREOPLATFORMER_API ATileHole : public AItemObstacle
{
	GENERATED_BODY()

protected:
	void PostObstacleActions() override;
};