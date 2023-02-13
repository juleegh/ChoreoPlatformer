// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* EnemyBody;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	virtual void BeginPlay() override;
	bool hasDoneTempoAction;
	virtual void DoTempoAction();
public:
	virtual void Tick(float DeltaTime) override;
};

UCLASS()
class CHOREOPLATFORMER_API ASplinedEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	ASplinedEnemy();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class USplineComponent* PatrolRoad;
	FVector GetWorldLocationByIndex(int Index) const;
	int GetLastIndex() const;
	UPROPERTY(BlueprintReadOnly)
	int PatrolIndex = -1;
}; 

UCLASS()
class CHOREOPLATFORMER_API AWalkingEnemy : public ASplinedEnemy
{
	GENERATED_BODY()

public:
	AWalkingEnemy() {}

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToLocation(FVector Location);
	void DoTempoAction() override;
};

UCLASS()
class CHOREOPLATFORMER_API ARotatingEnemy : public ASplinedEnemy
{
	GENERATED_BODY()

public:
	ARotatingEnemy() {}

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void RotateToLocation(FVector Location);
	void DoTempoAction() override;
};
