// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCameraComponent.h"
#include "DanceCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

UGameCameraComponent::UGameCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGameCameraComponent::SetupPlayerCamera(FGameplayTag CameraStyle)
{
	FPigeonCameraSettings CameraSettings = FlavorCameraSettings[FGameplayTag::RequestGameplayTag(FName("CameraSettings.Default"))];
	if (FlavorCameraSettings.Contains(CameraStyle))
	{
		CameraSettings = FlavorCameraSettings[CameraStyle];
	}

	auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
	DanceCharacter->GetMesh()->SetRelativeRotation(CameraSettings.PigeonRotation);
	if (CameraSettings.bForcePigeonLocation)
	{
		DanceCharacter->SetActorLocation(CameraSettings.PigeonWorldLocation);
	}
	if (UCameraComponent* PigeonCamera = Cast<UCameraComponent>(DanceCharacter->GetComponentByClass(UCameraComponent::StaticClass())))
	{
		PigeonCamera->SetRelativeRotation(CameraSettings.CameraRotation);
	}

	if (USpringArmComponent* SpringArm = Cast<USpringArmComponent>(DanceCharacter->GetComponentByClass(USpringArmComponent::StaticClass())))
	{
		SpringArm->TargetOffset = CameraSettings.TargetOffset;
		SpringArm->TargetArmLength = CameraSettings.TargetArmLength;
		SpringArm->SetRelativeRotation(CameraSettings.ArmRotation);
	}

}