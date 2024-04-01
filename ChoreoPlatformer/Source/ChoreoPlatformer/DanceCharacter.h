// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DanceDefinitions.h"
#include "DanceCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMoved, float, Tempo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerNewPosition);

USTRUCT(BlueprintType)
struct FPigeonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FVector TargetOffset;
	UPROPERTY(EditDefaultsOnly)
	float TargetArmLength;
	UPROPERTY(EditDefaultsOnly)
	FRotator ArmRotation;
	UPROPERTY(EditDefaultsOnly)
	FRotator CameraRotation;
	UPROPERTY(EditDefaultsOnly)
	FRotator PigeonRotation;
	UPROPERTY(EditDefaultsOnly)
	bool bForcePigeonLocation;
	UPROPERTY(EditDefaultsOnly)
	FVector PigeonWorldLocation;
};

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
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<EMoveResult, FColor> ReactionColors;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<EMoveResult, class UParticleSystem*> ReactionParticles;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<EMoveResult, class UAnimMontage*> ReactionAnimations;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TMap<FGameplayTag, FPigeonCameraSettings> FlavorCameraSettings;

	UPROPERTY()
	class UMovementTimelineComponent* MoveTimeline;
	UPROPERTY()
	class UColorTimelineComponent* ColorTimeline;
	UPROPERTY()
	class ADanceAudioManager* DanceAudio;
	UFUNCTION()
	virtual void BeginPlay() override;
	UFUNCTION()
	void ReachedNextTile();

private:

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerMoved PlayerMoved;
	UPROPERTY(BlueprintAssignable)
	FPlayerNewPosition PlayerNewPosition;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveTo(FVector position, float Duration);
	void RotateTowards(FVector position);
	void StopMovement();
	class AChoreoPlayerController* GetChoreoController() const;
	class UMovementTimelineComponent* GetMovementTimeline() { return MoveTimeline; }
	void InitializeToLevel(float Tempo);
	void SetupPlayerCamera(FGameplayTag CameraStyle);
	void ToggleReaction(EMoveResult MoveResult);
};
