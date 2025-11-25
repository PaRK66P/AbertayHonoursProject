// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GeometryRealisationComponent.generated.h"

struct FPathSection;

UENUM(BlueprintType)
enum class ENodeType : uint8
{
	Open UMETA(DisplayName = "Open"),
	Path UMETA(DisplayName = "Path"),
	Platform UMETA(DisplayName = "Platform")
};

USTRUCT(BlueprintType)
struct FNodeStruct
{
	GENERATED_USTRUCT_BODY()

public:
	FVector Position;
	ENodeType NodeType = ENodeType::Open;

	void SetNodeValues(FVector NodePosition, ENodeType ChosenNodeType) {
		Position = NodePosition;
		NodeType = ChosenNodeType;
	}
};

USTRUCT(BlueprintType)
struct FChunkStruct
{
	GENERATED_USTRUCT_BODY()

public:
	FVector ChunkOrigin;

	static const int32 ChunkSize = 16;
	FVector NodeDimensions;
	FVector ChunkDimensions;

	UPROPERTY(/*VisibleAnywhere, BlueprintReadWrite, Category = "Environment"*/)
	TArray<FNodeStruct> ChunkNodes;

	FNodeStruct ErrorNode;

	FChunkStruct() {
		ChunkNodes.SetNum(ChunkSize * ChunkSize * ChunkSize);
	}

	void SetPosition(FVector Position, FVector NewNodeDimensions) {
		ChunkOrigin = Position;
		NodeDimensions = NewNodeDimensions;
		ChunkDimensions = NodeDimensions * ChunkSize;

		FVector ChunkBottomLeft = ChunkOrigin - ChunkDimensions / 2.0f;
		FVector BottomLeftNodePosition = ChunkBottomLeft + NodeDimensions / 2.0f;

		for (int X = 0; X < ChunkSize; X++) {
			for (int Y = 0; Y < ChunkSize; Y++) {
				for (int Z = 0; Z < ChunkSize; Z++) {
					FNodeStruct& CurrentNode = GetNode(X, Y, Z);
					CurrentNode.Position =
						BottomLeftNodePosition +
						FVector(
							X * NodeDimensions.X,
							Y * NodeDimensions.Y,
							Z * NodeDimensions.Z);
				}
			}
		}
	}

	FNodeStruct& GetNode(int32 X, int32 Y, int32 Z)
	{
		if (X < 0 || X >= ChunkSize || Y < 0 || Y >= ChunkSize || Z < 0 || Z >= ChunkSize) {

			UE_LOG(LogTemp, Error,
				TEXT("Node out of range: %i, %i, %i"),
				X, Y, Z);
			return ErrorNode;
		}
		return ChunkNodes[
			X +
			(Y * ChunkSize) +
			(Z * ChunkSize * ChunkSize)];
	}

	FNodeStruct& GetNodeFromPosition(FVector NodePosition)
	{
		FVector ChunkBottomLeft = ChunkOrigin - (ChunkDimensions / 2.0f);

		FVector Distance = NodePosition - ChunkBottomLeft;

		// Normalise
		int32 X = FMath::FloorToInt(Distance.X / NodeDimensions.X);
		int32 Y = FMath::FloorToInt(Distance.Y / NodeDimensions.Y);
		int32 Z = FMath::FloorToInt(Distance.Z / NodeDimensions.Z);

		return GetNode(X, Y, Z);
	}

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UGeometryRealisationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGeometryRealisationComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Environment")
	TSubclassOf<AActor> StandardPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FVector GridOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FVector NodeDimensions;

	UPROPERTY()
	TArray<FChunkStruct> GeometryGrid;

	UPROPERTY()
	TArray<AActor*> GeneratedPlatforms;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector ChunkDimensions;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitialiseComponent();

	// Returns the end position for future extension of the Path
	FVector AddPathToGrid(TArray<FPathSection> Path, FVector PathOrigin);

	FChunkStruct& AddChunkToGrid(FVector ChunkPosition);
	FChunkStruct& GetChunkFromGrid(FVector ChunkPosition);
	FBox GetChunkBoxDimensions(FVector ChunkPosition);
	FVector GetCenterOfChunkInPosition(FVector ChunkPosition);

	void SetNodeAtPosition(FVector NodePosition, ENodeType NodeType);

	UFUNCTION()
	void GenerateLevel();
	UFUNCTION()
	void RemoveGeneratedLevel();
	UFUNCTION()
	void ClearGrid();

	void SpawnPlatformAtPosition(FVector Position);
		
};
