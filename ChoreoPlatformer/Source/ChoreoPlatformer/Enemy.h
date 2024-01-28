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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UPaperFlipbookComponent* NextPositionIndicator;
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	UPROPERTY()
	class AChoreoPlayerController* PlayerController;
	UPROPERTY()
	class ADanceCharacter* PlayerCharacter;
	UPROPERTY()
	class UMovementTimelineComponent* MoveTimeline;
	UPROPERTY()
	class UColorTimelineComponent* ColorTimeline;
	UPROPERTY()
	FGameplayTag Section = FGameplayTag::EmptyTag;
	UPROPERTY()
	class ASectionLevelManager* SectionLevelManager;

	bool hasDoneTempoAction;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void DoTempoAction();
	UFUNCTION(BlueprintImplementableEvent)
	void StartedWalking();
	UFUNCTION(BlueprintImplementableEvent)
	void StartedRotating(float bPositive);
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
	void MarkNextTarget();
}; 

UCLASS()
class CHOREOPLATFORMER_API AWalkingEnemy : public ASplinedEnemy
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
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
	void BeginPlay() override;
	void DoTempoAction() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* HitBox;
};
