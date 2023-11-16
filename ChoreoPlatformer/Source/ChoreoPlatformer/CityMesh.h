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

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UStaticMesh* SelectedMesh;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FColor ColorSlot1;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FColor ColorSlot2;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FColor ColorSlot3;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bOccluded;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bMetallic;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float OcclusionDistance = 400;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float OcclusionVolume = 700;

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* ObjectMat;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};