// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS(ClassGroup = (Custom))
class CHOREOPLATFORMER_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	void SetupEnemy();
	bool CurrentlyActive() const;
	bool CanMove() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UPaperSpriteComponent* AttackIndicator;
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	UPROPERTY()
	class AChoreoPlayerController* PlayerController;
	UPROPERTY()
	class UMovementTimelineComponent* MoveTimeline;
	UPROPERTY()
	class UColorTimelineComponent* ColorTimeline;
	UPROPERTY()
	class UScaleUpTimelineComponent* ScaleTimeline;
	UPROPERTY()
	FGameplayTag Section = FGameplayTag::EmptyTag;
	UPROPERTY()
	class ASectionLevelManager* SectionLevelManager;

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	virtual void DoTempoAction() {}
	virtual void DoDamage(FVector DamageArea);
	UFUNCTION(BlueprintImplementableEvent)
	void StartedWalking();
	UFUNCTION(BlueprintImplementableEvent)
	void StartedRotating(float bPositive);
	virtual void MarkNextTarget() {}
};

UCLASS()
class CHOREOPLATFORMER_API AForwardEnemy : public AEnemy
{
	GENERATED_BODY()

protected:
	void DoTempoAction() override;
	void MarkNextTarget() override;
	FVector GetNextTile(FVector Position);
};

UCLASS()
class CHOREOPLATFORMER_API ASplinedEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	ASplinedEnemy();

protected:
	void BeginPlay() override;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class USplineComponent* PatrolRoad;
	FVector GetWorldLocationByIndex(int Index) const;
	int GetLastIndex() const;
	UPROPERTY(BlueprintReadOnly)
	int PatrolIndex = -1;
	UPROPERTY()
	TArray<FVector> PatrolPoints;
	void MarkNextTarget() override;
}; 

UCLASS()
class CHOREOPLATFORMER_API AWalkingEnemy : public ASplinedEnemy
{
	GENERATED_BODY()

protected:
	void DoTempoAction() override;
	UFUNCTION()
	void LookAtNextTarget();
};

UCLASS()
class CHOREOPLATFORMER_API ARotatingEnemy : public ASplinedEnemy
{
	GENERATED_BODY()
public:
	ARotatingEnemy();
protected:
	void DoTempoAction() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* HitBox;
};
