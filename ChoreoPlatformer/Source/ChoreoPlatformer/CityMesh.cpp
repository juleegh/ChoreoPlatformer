#include "CityMesh.h"
#include "Components/StaticMeshComponent.h"
#include "TimelineCreatorComponent.h"

ACityMesh::ACityMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Prop Mesh"));
	ObjectMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = ObjectMesh;
	TilingMeshes.Add(ObjectMesh);
	MoveTimeline = CreateDefaultSubobject<UMovementTimelineComponent>("Move Timeline");
}

void ACityMesh::BeginPlay()
{
	Super::BeginPlay();
	MoveTimeline->Initialize();
}

void ACityMesh::UpdatedMesh()
{
	if (!SelectedMesh)
	{
		return;
	}

#if WITH_EDITOR
	SetActorLabel(SelectedMesh->GetFName().ToString());
#endif
	PaintMaterial();
	CheckTilingUnits();
	CheckTilingColoring();
	CheckTilingPositioning();

	for (auto Tiled : TilingMeshes)
	{
		Tiled->SetStaticMesh(SelectedMesh);
	}
}

#if WITH_EDITOR
void ACityMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdatedMesh();
}
#endif

void ACityMesh::PaintMaterial()
{
	if (!ObjectMat)
	{
		ObjectMat = ObjectMesh->CreateDynamicMaterialInstance(0, ObjectMesh->GetMaterial(0));
		TilingMats.Add(ObjectMat);
	}
	ObjectMat->SetVectorParameterValue(FName("First"), ColorSlot1);
	ObjectMat->SetVectorParameterValue(FName("Second"), ColorSlot2);
	ObjectMat->SetVectorParameterValue(FName("Third"), ColorSlot3);
	ObjectMat->SetScalarParameterValue(FName("ShouldOclude"), bOccluded);
	ObjectMat->SetScalarParameterValue(FName("OcclusionDistance"), OcclusionDistance);
	ObjectMat->SetScalarParameterValue(FName("OcclusionVolume"), OcclusionVolume);
	ObjectMat->SetScalarParameterValue(FName("Metallic"), bMetallic);
}

void ACityMesh::CheckTilingUnits()
{
	if (Width < 1)
	{
		Width = 1;
	}
	if (Height < 1)
	{
		Height = 1;
	}

	while (Width * Height > TilingMeshes.Num())
	{
		auto Tiled = NewObject<UStaticMeshComponent>(this);
		Tiled->RegisterComponent();
		Tiled->SetCollisionResponseToAllChannels(ECR_Ignore);
		Tiled->SetMaterial(0, ObjectMat);
		Tiled->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		TilingMeshes.Add(Tiled);
	}

	while (Width * Height < TilingMeshes.Num())
	{
		auto Tiled = TilingMeshes.Last();
		TilingMeshes.Remove(Tiled);
		Tiled->DestroyComponent();
	}
}

void ACityMesh::CheckTilingColoring()
{
	if (ChromaticAberration1Threshold <= 0 && ChromaticAberration2Threshold <= 0 && ChromaticAberration3Threshold <= 0)
	{
		if (TilingMeshes.Num() > 1)
		{
			TilingMats.Empty();
			TilingMats.Add(ObjectMat);
		}
	}
	else
	{
		if (Width * Height != TilingMeshes.Num())
		{
			TilingMats.Empty();
			TilingMats.Add(ObjectMat);

			for (auto Tiled : TilingMeshes)
			{
				if (Tiled == ObjectMesh)
					continue;

				auto TiledMat = Tiled->CreateDynamicMaterialInstance(0, ObjectMaterial);
				TiledMat->SetScalarParameterValue(FName("ShouldOclude"), bOccluded);
				TiledMat->SetScalarParameterValue(FName("OcclusionDistance"), OcclusionDistance);
				TiledMat->SetScalarParameterValue(FName("OcclusionVolume"), OcclusionVolume);
				TiledMat->SetScalarParameterValue(FName("Metallic"), bMetallic);

				Tiled->SetMaterial(0, TiledMat);
				TilingMats.Add(TiledMat);
			}
		}

		for (auto TiledMat : TilingMats)
		{
			TiledMat->SetVectorParameterValue(FName("First"), ColorSlot1);
			TiledMat->SetVectorParameterValue(FName("Second"), ColorSlot2);
			TiledMat->SetVectorParameterValue(FName("Third"), ColorSlot3);

			if (ChromaticAberration1Threshold > 0)
			{
				TiledMat->SetVectorParameterValue(FName("First"), FColor(ColorSlot1.R + FMath::RandRange(-ChromaticAberration1Threshold, ChromaticAberration1Threshold), ColorSlot1.G + FMath::RandRange(-ChromaticAberration1Threshold, ChromaticAberration1Threshold), ColorSlot1.B + FMath::RandRange(-ChromaticAberration1Threshold, ChromaticAberration1Threshold), 1));
			}
			if (ChromaticAberration2Threshold > 0)
			{
				TiledMat->SetVectorParameterValue(FName("Second"), FColor(ColorSlot2.R + FMath::RandRange(-ChromaticAberration2Threshold, ChromaticAberration2Threshold), ColorSlot2.G + FMath::RandRange(-ChromaticAberration2Threshold, ChromaticAberration2Threshold), ColorSlot2.B + FMath::RandRange(-ChromaticAberration2Threshold, ChromaticAberration2Threshold), 1));
			}
			if (ChromaticAberration3Threshold > 0)
			{
				TiledMat->SetVectorParameterValue(FName("Third"), FColor(ColorSlot3.R + FMath::RandRange(-ChromaticAberration3Threshold, ChromaticAberration3Threshold), ColorSlot3.G + FMath::RandRange(-ChromaticAberration3Threshold, ChromaticAberration3Threshold), ColorSlot3.B + FMath::RandRange(-ChromaticAberration3Threshold, ChromaticAberration3Threshold), 1));
			}
		}
	}
}

void ACityMesh::CheckTilingPositioning()
{
	float column = 0;
	float row = 0;

	for (auto Tiled : TilingMeshes)
	{
		if (Tiled != ObjectMesh)
		{
			float PosDeltaX = bRandomPositionDelta ? FMath::RandRange(-PositionDeltaThreshold, PositionDeltaThreshold) : 0;
			float PosDeltaY = bRandomPositionDelta ? FMath::RandRange(-PositionDeltaThreshold, PositionDeltaThreshold) : 0;
			FVector Position = FVector(column * XDistance + PosDeltaX, bChangeTilingOrientation ? row * YDistance + PosDeltaY : 0, bChangeTilingOrientation ? 0 : row * YDistance + PosDeltaY);
			Tiled->SetRelativeLocation(Position);
			Tiled->SetRelativeRotation(FRotator(0));
			if (bRandomRotation || bRandomRectangularRotation)
			{
				float Rot = FMath::RandRange(0, 360);
				if (bRandomRectangularRotation)
				{
					int Angle = Rot / 90;
					Rot = Angle * 90;
				}
				Tiled->SetRelativeRotation(FRotator(0, Rot, 0));
			}
		}
		column++;
		if (column >= Width)
		{
			column = 0;
			row++;
		}
	}
}

void ACityMesh::MoveToPosition(FVector Position, float elapsedTime)
{
	if (MoveTimeline->IsRunning())
	{
		MoveTimeline->Stop(true);
	}
	MoveTimeline->MoveToPosition(Position, elapsedTime);
}
