// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "GameCameraComponent.generated.h"

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
class CHOREOPLATFORMER_API UGameCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGameCameraComponent();
	
	UPROPERTY()
	TMap<FGameplayTag, FPigeonCameraSettings> FlavorCameraSettings;
	void SetupPlayerCamera(FGameplayTag CameraStyle);
};
