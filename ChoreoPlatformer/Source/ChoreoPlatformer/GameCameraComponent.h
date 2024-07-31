// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "DanceDefinitions.h"
#include "GameCameraComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCameraToggled);

UCLASS()
class CHOREOPLATFORMER_API UGameCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGameCameraComponent();
	
protected:
	UPROPERTY()
	TMap<FGameplayTag, class APictureCameraActor*> PictureCameras;
	UPROPERTY()
	FVector RelativePosition;
	UPROPERTY()
	FVector RelativeHeight;
	UPROPERTY()
	float LastDelta;
	UPROPERTY()
	bool bIsMovingCamera = false;
	UPROPERTY()
	float CameraSpeed = 1000.f;
	UPROPERTY()
	class UCameraComponent* PigeonCamera;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintAssignable)
	FCameraToggled CameraToggled;

	void InitializeCameras();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsMovingCamera() const;
	void ToggleCameraMovement(bool bMove);
	UFUNCTION(BlueprintCallable)
	void MoveCamera();
	void SetupPlayerCamera(FGameplayTag CameraId);
};

UCLASS()
class CHOREOPLATFORMER_API APictureCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	APictureCameraActor() {}
	FGameplayTag& GetLevelTag();
	FPigeonCameraSettings& GetCameraSettings();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FGameplayTag LevelTag;
	UPROPERTY(EditInstanceOnly)
	FPigeonCameraSettings FlavorCameraSettings;
};
