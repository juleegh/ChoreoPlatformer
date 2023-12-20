#include "CityMesh.h"
#include "Components/StaticMeshComponent.h"

ACityMesh::ACityMesh()
{
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Prop Mesh"));
	ObjectMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = ObjectMesh;
	TilingMeshes.Add(ObjectMesh);
}

#if WITH_EDITOR
void ACityMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!SelectedMesh)
	{
		return;
	}

	SetActorLabel(SelectedMesh->GetFName().ToString());
	PaintMaterial();
	CheckTiling();
	
	for (auto Tiled : TilingMeshes)
	{
		Tiled->SetStaticMesh(SelectedMesh);
	}
}
#endif

void ACityMesh::PaintMaterial()
{
	if (!ObjectMat)
	{
		ObjectMat = ObjectMesh->CreateDynamicMaterialInstance(0, ObjectMesh->GetMaterial(0));
	}
	ObjectMat->SetVectorParameterValue(FName("First"), ColorSlot1);
	ObjectMat->SetVectorParameterValue(FName("Second"), ColorSlot2);
	ObjectMat->SetVectorParameterValue(FName("Third"), ColorSlot3);
	ObjectMat->SetScalarParameterValue(FName("ShouldOclude"), bOccluded);
	ObjectMat->SetScalarParameterValue(FName("OcclusionDistance"), OcclusionDistance);
	ObjectMat->SetScalarParameterValue(FName("OcclusionVolume"), OcclusionVolume);
	ObjectMat->SetScalarParameterValue(FName("Metallic"), bMetallic);
}

void ACityMesh::CheckTiling()
{
	if (Width < 1)
	{
		Width = 1;
	}
	if (Height < 1)
	{
		Height = 1;
	}

	while (Width*Height > TilingMeshes.Num())
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

	float column = 0;
	float row = 0;

	for (auto Tiled : TilingMeshes)
	{
		if (Tiled != ObjectMesh)
		{
			Tiled->SetRelativeLocation(FVector(column * XDistance, 0, row * YDistance));
		}
		column++;
		if (column >= Width)
		{
			column = 0;
			row++;
		}
	}
	
}
