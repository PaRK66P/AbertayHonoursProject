// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryRealisationComponent.h"

// Sets default values for this component's properties
UGeometryRealisationComponent::UGeometryRealisationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGeometryRealisationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (StandardPlatform) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = GetOwner();
		AActor* platformSpawned = GetWorld()->SpawnActor<AActor>(
			StandardPlatform,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation(),
			spawnParams);

		if (platformSpawned) {
			platformSpawned->AttachToActor(
				GetOwner(),
				FAttachmentTransformRules::KeepRelativeTransform
			);
		}
	}

	
}


// Called every frame
void UGeometryRealisationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UGeometryRealisationComponent::AddPathToGrid(TArray<FGeneratedBeatValues> Path, FVector PathOrigin)
{
	return FVector();
}



