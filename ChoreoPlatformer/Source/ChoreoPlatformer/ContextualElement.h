// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ContextualElement.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AContextualElement : public AActor
{
	GENERATED_BODY()

public:
	AContextualElement();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;

public:
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
	ADoor* ConnectedDoor;
public:
	ALever() {}
	void TriggerInteraction() override;
};