// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChoreoPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerMoved);

UCLASS()
class CHOREOPLATFORMER_API AChoreoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	class USongTempoComponent* SongTempo;
	UPROPERTY()
	class ULevelProgressComponent* LevelProgress;
	UPROPERTY()
	class UDancerHealthComponent* DancerHealth;
	UPROPERTY()
	class UDancerUIComponent* DancerUI;
	UPROPERTY()
	class ASectionLevelManager* SectionManager;
	UPROPERTY()
	class ADanceCharacter* DanceCharacter;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	int IntroductionTempos = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bShouldTakeDamage = false;
	UPROPERTY(EditDefaultsOnly, Category = "Debugging")
	bool bShouldPunishTempo = false;
	void CheckMovement(FVector Direction);
	UFUNCTION()
	void OnFinishedMovement();
	UFUNCTION()
	void OnPlayerDied();

public:
	AChoreoPlayerController();
	UPROPERTY(BlueprintAssignable)
	FPlayerMoved PlayerMoved;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class USongTempoComponent* GetSongTempoComponent() const { return SongTempo; }
	class ULevelProgressComponent* GetLevelProgressComponent() const { return LevelProgress; }
	class UDancerHealthComponent* GetDancerHealthComponent() const { return DancerHealth; }
	void PressedUp();
	void PressedDown();
	void PressedLeft();
	void PressedRight();
	bool ShouldTakeDamage() { return bShouldTakeDamage; }
	virtual void Tick(float DeltaTime) override;
};
