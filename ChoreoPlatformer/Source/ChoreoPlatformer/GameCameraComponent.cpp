// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCameraComponent.h"
#include "DanceCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

UGameCameraComponent::UGameCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
		PigeonCamera->SetRelativeLocation(RelativePosition);
	}

	if (USpringArmComponent* SpringArm = Cast<USpringArmComponent>(DanceCharacter->GetComponentByClass(USpringArmComponent::StaticClass())))
	{
		SpringArm->TargetOffset = CameraSettings.TargetOffset;
		SpringArm->TargetArmLength = CameraSettings.TargetArmLength;
		SpringArm->SetRelativeRotation(CameraSettings.ArmRotation);
	}

}

void UGameCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	LastDelta = DeltaTime;
}

bool UGameCameraComponent::IsMovingCamera() const
{
	return bIsMovingCamera;
}

void UGameCameraComponent::ToggleCameraMovement(bool bMove)
{
	bIsMovingCamera = bMove;
	if (!bIsMovingCamera)
	{
		SetupPlayerCamera(FGameplayTag::EmptyTag);
	}
	else
	{
		auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
		if (UCameraComponent* PigeonCamera = Cast<UCameraComponent>(DanceCharacter->GetComponentByClass(UCameraComponent::StaticClass())))
		{
			RelativePosition = PigeonCamera->GetRelativeLocation();
		}
	}
}

void UGameCameraComponent::MoveCamera()
{
	auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
	FVector Direction = FVector(-DanceCharacter->GetCurrentInput().Y, DanceCharacter->GetCurrentInput().X, 0);
	FVector Delta = Direction * CameraSpeed * LastDelta;

	if (UCameraComponent* PigeonCamera = Cast<UCameraComponent>(DanceCharacter->GetComponentByClass(UCameraComponent::StaticClass())))
	{
		FVector StartLocation = PigeonCamera->GetComponentLocation();
		FVector EndLocation = StartLocation + Delta * 2;

		float Radius = 100.0f;
		ECollisionChannel TraceChannel = ECC_WorldStatic;
		FCollisionQueryParams TraceParams = FCollisionQueryParams::DefaultQueryParam;

		FHitResult HitResult;
		bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), TraceParams);

		if (!bHit)
		{
			PigeonCamera->AddRelativeLocation(Delta, true, nullptr, ETeleportType::None);
		}
	}
}