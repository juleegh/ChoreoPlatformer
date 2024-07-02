// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DanceDefinitions.h"
#include "GameCameraComponent.h"
#include "DanceCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMoved, float, Tempo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerNewPosition);

struct FInputActionValue;

UCLASS()
class CHOREOPLATFORMER_API ADanceCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADanceCharacter();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* ChoreoIMC;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* PauseAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ConfirmAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* CancelAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* CameraAction;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<EMoveResult, FColor> ReactionColors;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<EMoveResult, class UParticleSystem*> ReactionParticles;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<EMoveResult, class UAnimMontage*> ReactionAnimations;

	UPROPERTY()
	class UMovementTimelineComponent* MoveTimeline;
	UPROPERTY()
	class UColorTimelineComponent* ColorTimeline;
	UPROPERTY()
	UGameCameraComponent* GameCamera;
	UPROPERTY()
	class ADanceAudioManager* DanceAudio;
	UFUNCTION()
	void BeginPlay() override;
	UFUNCTION()
	void ReachedNextTile();

private:
	void MovePressed(const FInputActionValue& Value);
	void MoveTriggered(const FInputActionValue& Value);
	void MoveReleased(const FInputActionValue& Value);
	void MoveCamera(const FInputActionValue& Value);
	void StopCamera(const FInputActionValue& Value);
	UPROPERTY()
	FVector InputDirection;
	UPROPERTY()
	float CurrentMovementSpeed;

public:
	FVector GetCurrentInput() const;
	void ClearInput();
	UPROPERTY(BlueprintAssignable)
	FPlayerMoved PlayerMoved;
	UPROPERTY(BlueprintAssignable)
	FPlayerNewPosition PlayerNewPosition;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveTo(FVector position, float Duration);
	void RotateTowards(FVector position);
	void StopMovement();
	void Respawn(FVector Position);
	class UMovementTimelineComponent* GetMovementTimeline() { return MoveTimeline; }
	void InitializeToLevel(float Tempo);
	void ToggleReaction(EMoveResult MoveResult);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMovementSpeed() const;
};
