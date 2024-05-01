// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "GameCameraComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCameraToggled);

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
	
protected:
	UPROPERTY()
	FVector RelativePosition;
	UPROPERTY()
	float LastDelta;
	UPROPERTY()
	bool bIsMovingCamera = false;
	UPROPERTY()
	float CameraSpeed = 1000.f;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	TMap<FGameplayTag, FPigeonCameraSettings> FlavorCameraSettings;
	UPROPERTY(BlueprintAssignable)
	FCameraToggled CameraToggled;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsMovingCamera() const;
	void ToggleCameraMovement(bool bMove);
	UFUNCTION(BlueprintCallable)
	void MoveCamera();
	void SetupPlayerCamera(FGameplayTag CameraStyle);
};
