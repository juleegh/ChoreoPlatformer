// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityMesh.generated.h"

UCLASS()
class CHOREOPLATFORMER_API ACityMesh : public AActor
{
	GENERATED_BODY()

public:
	ACityMesh();
	void UpdatedMesh();
	void MoveToPosition(FVector Position, float elapsedTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* ObjectMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UMaterial* ObjectMaterial;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	UStaticMesh* SelectedMesh;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	FColor ColorSlot1;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	FColor ColorSlot2;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	FColor ColorSlot3;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	bool bOccluded;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	bool bMetallic;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	float OcclusionDistance = 400;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Material")
	float OcclusionVolume = 700;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Tiling")
	int Width = 1;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Tiling")
	float XDistance = 100;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Tiling")
	int Height = 1;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Tiling")
	float YDistance = 200;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Tiling")
	bool bChangeTilingOrientation = false;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	bool bRandomRectangularRotation = false;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	bool bRandomRotation = false;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	bool bRandomPositionDelta = false;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	float PositionDeltaThreshold = 10;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	float ChromaticAberration1Threshold = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	float ChromaticAberration2Threshold = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Randomness")
	float ChromaticAberration3Threshold = 0;

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* ObjectMat;
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> TilingMats;
	UPROPERTY()
	TArray<UStaticMeshComponent*> TilingMeshes;
	UPROPERTY()
	class UMovementTimelineComponent* MoveTimeline;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void BeginPlay() override;
	void PaintMaterial();
	void CheckTilingUnits();
	void CheckTilingPositioning();
	void CheckTilingColoring();
};