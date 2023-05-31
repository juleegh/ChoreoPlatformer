// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileChallenge.generated.h"

UENUM(BlueprintType)
enum class EChallengeType : uint8
{
	HalfCoin,
	CoinTrail,
	CoinStop,
};

UCLASS()
class CHOREOPLATFORMER_API ATileChallenge : public AActor
{
	GENERATED_BODY()
	
public:	
	ATileChallenge();

protected:
	UPROPERTY()
	EChallengeType ChallengeType;
	UPROPERTY()
	bool bUnderProgress;
	UPROPERTY()
	bool bCompleted;
	UPROPERTY()
	int TemposWithoutMoving;
	UPROPERTY()
	class ADanceCharacter* Player;
	UPROPERTY()
	class UDancerUIComponent* PlayerUI;
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class USplineComponent* InterestPoints;
	
	virtual void BeginPlay() override;
	int GetPlayerCurrentIndex();
	virtual void StartChallenge() {}
	UFUNCTION()
	virtual void TempoHasPassed();
	UFUNCTION()
	virtual void PlayerChangedPosition();
	UFUNCTION(BlueprintCallable)
	bool IsUndergoing();
	void EndChallenge(bool bWasSuccessful);
	UFUNCTION(BlueprintImplementableEvent)
	void ChallengeStarted(); 
	UFUNCTION(BlueprintImplementableEvent)
	void ChallengeUpdated();
	UFUNCTION(BlueprintImplementableEvent)
	void ChallengeEnded(bool bWasSuccessful);
};

UCLASS()
class CHOREOPLATFORMER_API AHalfCoin : public ATileChallenge
{
	GENERATED_BODY()

public:
	AHalfCoin() { ChallengeType = EChallengeType::HalfCoin; }

protected:
	UPROPERTY(BlueprintReadOnly)
	int TemposRemaining;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int TemposBetweenHalfs;
	void StartChallenge() override;
	void TempoHasPassed() override;
};

UCLASS()
class CHOREOPLATFORMER_API ACoinTrail : public ATileChallenge
{
	GENERATED_BODY()

public:
	ACoinTrail() { ChallengeType = EChallengeType::CoinTrail; }

protected:
	UPROPERTY(BlueprintReadOnly)
	int CurrentCoin;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int TemposBetweenHalfs;
	void StartChallenge() override;
	void TempoHasPassed() override;
	void PlayerChangedPosition() override;
};

UCLASS()
class CHOREOPLATFORMER_API ACoinStop : public ATileChallenge
{
	GENERATED_BODY()

public:
	ACoinStop() { ChallengeType = EChallengeType::CoinStop; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int TemposInStop;
	void PlayerChangedPosition() override;
	void TempoHasPassed() override;
};
