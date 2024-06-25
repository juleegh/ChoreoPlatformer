// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DanceCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

UGameCameraComponent::UGameCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGameCameraComponent::InitializeCameras()
{
	PictureCameras.Empty();
	TArray<AActor*> PictureCamerasActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APictureCameraActor::StaticClass(), PictureCamerasActors);
	for (auto Actor : PictureCamerasActors)
	{
		if (auto PictureCamera = Cast<APictureCameraActor>(Actor))
		{
			PictureCameras.Add(PictureCamera->GetLevelTag(), PictureCamera);
		}
	}
}

void UGameCameraComponent::SetupPlayerCamera(FGameplayTag CameraId)
{
	auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
	DanceCharacter->SetActorRotation(FRotator::ZeroRotator);
	if (!PictureCameras.Contains(CameraId))
	{
		DanceCharacter->GetMesh()->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0,0,-90)));
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(DanceCharacter, 0.f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
		return;
	}
	FPigeonCameraSettings CameraSettings = PictureCameras[CameraId]->GetCameraSettings();
	DanceCharacter->GetMesh()->SetRelativeRotation(CameraSettings.PigeonRotation);
	if (CameraSettings.bForcePigeonLocation)
	{
		DanceCharacter->SetActorLocation(CameraSettings.PigeonWorldLocation);
	}
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(PictureCameras[CameraId], 0.25f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
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
	CameraToggled.Broadcast();
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

FGameplayTag& APictureCameraActor::GetLevelTag()
{
	return LevelTag;
}

FPigeonCameraSettings& APictureCameraActor::GetCameraSettings()
{
	return FlavorCameraSettings;
}