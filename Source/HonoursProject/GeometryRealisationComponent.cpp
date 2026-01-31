// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryRealisationComponent.h"

#include "PathRealisation.h"
#include "ActionGrammarsHolder.h"

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
}


// Called every frame
void UGeometryRealisationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGeometryRealisationComponent::InitialiseComponent()
{
	FChunkStruct base;
	ChunkDimensions = NodeDimensions * base.ChunkSize;
}

/* FUTURE TO DO
* Right now being lazy and not going to implement the path nodes
* Path nodes are implemented to prevent environmental objects blocking the traversal path
* Since we don't have environmental objects yet I've not implemented this
*/
FVector UGeometryRealisationComponent::AddPathToGrid(TArray<FPathSection> Path, FVector PathOrigin)
{

	//SetNodeAtPosition(PathOrigin, ENodeType::Platform);

	//FVector lineDistance;

	for (FPathSection PathSection : Path) {
		switch (PathSection.SectionType)
		{
		case EPathSectionType::Move:
			UE_LOG(LogTemp, Warning, TEXT("Drawing Line"));
			DrawDebugLine(GetWorld(), PathSection.StartPosition, PathSection.EndPosition, FColor::Red, true, -1.0f, (uint8)0U, 10.0f);
			break;
		case EPathSectionType::Jump:
			UE_LOG(LogTemp, Warning, TEXT("Drawing Line"));
			DrawDebugLine(GetWorld(), PathSection.StartPosition, PathSection.EndPosition, FColor::Green, true, -1.0f, (uint8)0U, 10.0f);
			break;
		default:
			break;
		}
	}

	return Path[Path.Num() - 1].EndPosition;
}

// Return value is mainly for the Get function bellow
FChunkStruct& UGeometryRealisationComponent::AddChunkToGrid(FVector ChunkPosition)
{
	FBox chunkBox;

	for (FChunkStruct& ExistingChunk : GeometryGrid) {
		chunkBox = GetChunkBoxDimensions(ExistingChunk.ChunkOrigin);
		if (chunkBox.IsInside(ChunkPosition)) {
			//Chunk exists
			return ExistingChunk;
		}
	}

	FChunkStruct newChunk;
	newChunk.SetPosition(GetCenterOfChunkInPosition(ChunkPosition), NodeDimensions);
	int32 ChunkIndex = GeometryGrid.Add(newChunk);

	return GeometryGrid[ChunkIndex];
}

FChunkStruct& UGeometryRealisationComponent::GetChunkFromGrid(FVector ChunkPosition)
{
	FBox chunkBox;

	for (FChunkStruct& ExistingChunk : GeometryGrid) {
		chunkBox = GetChunkBoxDimensions(ExistingChunk.ChunkOrigin);
		if (chunkBox.IsInside(ChunkPosition)) {
			return ExistingChunk;
		}
	}

	return AddChunkToGrid(ChunkPosition);
}

FBox UGeometryRealisationComponent::GetChunkBoxDimensions(FVector ChunkPosition)
{
	FVector NormalisedChunkPosition = GetCenterOfChunkInPosition(ChunkPosition);

	FVector chunkBottomLeft = NormalisedChunkPosition - ChunkDimensions / 2.0f;
	FVector chunkTopRight = NormalisedChunkPosition + ChunkDimensions / 2.0f;

	FBox chunkBox(chunkBottomLeft, chunkTopRight);
	return chunkBox;
}

FVector UGeometryRealisationComponent::GetCenterOfChunkInPosition(FVector ChunkPosition)
{
	// Normalise position to be on the grid
	FVector Distance = ChunkPosition - GridOrigin;

	if (Distance.X * Distance.X < (ChunkDimensions.X / 2.0f) * (ChunkDimensions.X / 2.0f) &&
		Distance.Y * Distance.Y < (ChunkDimensions.Y / 2.0f) * (ChunkDimensions.Y / 2.0f) &&
		Distance.Z * Distance.Z < (ChunkDimensions.Z / 2.0f) * (ChunkDimensions.Z / 2.0f)) {
		return GridOrigin;
	}

	FVector DistanceSign = FVector(FMath::Sign(Distance.X), FMath::Sign(Distance.Y), FMath::Sign(Distance.Z));

	Distance -= DistanceSign * ChunkDimensions / 2.0f;

	int32 ChunkX = FMath::FloorToInt(Distance.X / ChunkDimensions.X) + DistanceSign.X;
	int32 ChunkY = FMath::FloorToInt(Distance.Y / ChunkDimensions.Y) + DistanceSign.Y;
	int32 ChunkZ = FMath::FloorToInt(Distance.Z / ChunkDimensions.Z) + DistanceSign.Z;

	FVector ChunkCenter(
		GridOrigin.X + ChunkX * ChunkDimensions.X,
		GridOrigin.Y + ChunkY * ChunkDimensions.Y,
		GridOrigin.Z + ChunkZ * ChunkDimensions.Z
	);

	return ChunkCenter;
}

void UGeometryRealisationComponent::SetNodeAtPosition(FVector NodePosition, ENodeType NodeType)
{
	FChunkStruct& CurrentChunk = GetChunkFromGrid(NodePosition);

	FNodeStruct& CurrentNode = CurrentChunk.GetNodeFromPosition(NodePosition);

	CurrentNode.NodeType = NodeType;
}

void UGeometryRealisationComponent::GenerateLevel()
{
	RemoveGeneratedLevel();
	for (FChunkStruct Chunk : GeometryGrid) {
		for (FNodeStruct Node : Chunk.ChunkNodes) {
			if (Node.NodeType == ENodeType::Open) {
				continue;
			}

			switch (Node.NodeType)
			{
			case ENodeType::Platform:
				SpawnPlatformAtPosition(Node.Position);
				break;
			default:
				break;
			}
		}
	}
}

void UGeometryRealisationComponent::RemoveGeneratedLevel()
{
	for (AActor* Platform : GeneratedPlatforms) {
		Platform->Destroy();
	}

	GeneratedPlatforms.Empty();
}

void UGeometryRealisationComponent::ClearGrid()
{
	GeometryGrid.Empty();
}

void UGeometryRealisationComponent::SpawnPlatformAtPosition(FVector Position)
{
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = GetOwner();
	AActor* spawnedPlatform = GetWorld()->SpawnActor<AActor>(
		StandardPlatform,
		Position,
		FRotator::ZeroRotator,
		spawnParams);

	spawnedPlatform->AttachToActor(
		GetOwner(),
		FAttachmentTransformRules::KeepRelativeTransform
	);

	GeneratedPlatforms.Add(spawnedPlatform);
}



