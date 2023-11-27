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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* ObjectMesh;

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

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* ObjectMat;
	UPROPERTY()
	TArray<UStaticMeshComponent*> TilingMeshes;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void PaintMaterial();
	void CheckTiling();
};