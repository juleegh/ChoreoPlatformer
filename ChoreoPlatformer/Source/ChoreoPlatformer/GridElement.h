// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridElement.generated.h"

UCLASS()
class CHOREOPLATFORMER_API AGridElement : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridElement();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;

	UFUNCTION()
	void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnEnterRange() {};
	virtual void OnExitRange() {};
public:	
	//virtual void Tick(float DeltaTime) override;
};

UCLASS()
class CHOREOPLATFORMER_API ACheckpoint : public AGridElement
{
	GENERATED_BODY()

public:
	ACheckpoint() {}

protected:
	void OnEnterRange() override;
};

UCLASS()
class CHOREOPLATFORMER_API AMine : public AGridElement
{
	GENERATED_BODY()

public:
	AMine() {}

protected:
	void OnEnterRange() override;
};
