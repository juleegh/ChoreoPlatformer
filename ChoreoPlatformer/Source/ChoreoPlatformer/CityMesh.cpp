#include "CityMesh.h"
#include "Components/StaticMeshComponent.h"

ACityMesh::ACityMesh()
{
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Prop Mesh"));
	ObjectMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = ObjectMesh;
}

void ACityMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (SelectedMesh)
	{
		ObjectMesh->SetStaticMesh(SelectedMesh);
		SetActorLabel(SelectedMesh->GetFName().ToString());
	}
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
