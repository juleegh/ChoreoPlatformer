// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DanceCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DanceUtilsFunctionLibrary.h"

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
	PigeonCamera = Cast<UCameraComponent>(GetWorld()->GetFirstPlayerController()->GetPawn()->GetComponentByClass(UCameraComponent::StaticClass()));
}

void UGameCameraComponent::SetupPlayerCamera(FGameplayTag CameraId)
{
	auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
	DanceCharacter->SetActorRotation(FRotator::ZeroRotator);
	if (!PictureCameras.Contains(CameraId))
	{
		DanceCharacter->GetMesh()->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, 0, -90)));
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(DanceCharacter, 0.f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
		PigeonCamera->SetRelativeLocation(RelativePosition);
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
		RelativePosition = PigeonCamera->GetRelativeLocation();
		auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
		RelativeHeight = FVector::UpVector * (PigeonCamera->GetComponentLocation() - DanceCharacter->GetActorLocation()).Z;
	}
	CameraToggled.Broadcast();
}

void UGameCameraComponent::MoveCamera()
{
	auto DanceCharacter = Cast<ADanceCharacter>(GetOwner());
	FVector Direction = FVector(-DanceCharacter->GetCurrentInput().Y, DanceCharacter->GetCurrentInput().X, 0);
	FVector PositionDelta = Direction * CameraSpeed * LastDelta;
	FVector EndLocation = PigeonCamera->GetComponentLocation() + PositionDelta * 2;
	FVector HeightPosition = PigeonCamera->GetRelativeLocation().Z * FVector::UpVector;
	FVector HeightDelta = FVector::Zero();

	auto DownCheck = UDanceUtilsFunctionLibrary::CheckPosition({DanceCharacter}, EndLocation);
	if (DownCheck.HitCell)
	{
		HeightDelta = DownCheck.HitCell->GetActorLocation().Z * FVector::UpVector - HeightPosition;
	}

	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, PigeonCamera->GetComponentLocation(), EndLocation, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, FCollisionShape::MakeSphere(50.f), FCollisionQueryParams::DefaultQueryParam);
	if (!bHit)
	{
		PigeonCamera->AddRelativeLocation(PositionDelta + HeightDelta, true, nullptr, ETeleportType::None);
	}
	else
	{
		PigeonCamera->AddRelativeLocation(-HitResult.ImpactNormal * CameraSpeed *0.5f * LastDelta, true, nullptr, ETeleportType::None);
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