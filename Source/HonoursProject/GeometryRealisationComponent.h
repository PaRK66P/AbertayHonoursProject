// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GeometryRealisationComponent.generated.h"

struct FPathSection;
class UActionGrammarsHolder;

UENUM(BlueprintType)
enum class ENodeType : uint8
{
	Open UMETA(DisplayName = "Open"),
	Path UMETA(DisplayName = "Path"), // Area player travels across
	Platform UMETA(DisplayName = "Platform")
};

USTRUCT(BlueprintType)
struct FNodeStruct
{
	GENERATED_USTRUCT_BODY()

public:
	FVector Position;
	ENodeType NodeType = ENodeType::Open;
	bool isBlocking = false;

	void SetNodeValues(FVector NodePosition, ENodeType ChosenNodeType) {
		Position = NodePosition;
		NodeType = ChosenNodeType;
	}
};

USTRUCT(BlueprintType)
struct FGenerationPlatform
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FVector startPosition;
	UPROPERTY()
	FVector endPosition;
	UPROPERTY()
	FRotator rotation;
	UPROPERTY()
	AActor* platformRef;
};

USTRUCT(BlueprintType)
struct FGenerationCollectables
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FVector position;
	UPROPERTY()
	FRotator rotation;
	UPROPERTY()
	AActor* collectableRef;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	bool IncludeSafetyPlatforms = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Environment")
	TSubclassOf<AActor> StandardPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSubclassOf<AActor> StandardCollectable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FVector GridOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FVector NodeDimensions = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY()
	TArray<FChunkStruct> GeometryGrid;

	UPROPERTY()
	TArray<FGenerationPlatform> GeneratedPlatforms;

	UPROPERTY()
	TArray<FGenerationCollectables> GeneratedCollectables;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector ChunkDimensions;
	UActionGrammarsHolder* ActionGrammarsRef;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitialiseComponent(UActionGrammarsHolder* ref);

	// Returns the end position for future extension of the Path
	FVector AddPathToGrid(TArray<FPathSection> Path, FVector PathOrigin, FVector& FacingDirection);

	FChunkStruct& AddChunkToGrid(FVector ChunkPosition);
	FChunkStruct& GetChunkFromGrid(FVector ChunkPosition);
	FBox GetChunkBoxDimensions(FVector ChunkPosition);
	FVector GetCenterOfChunkInPosition(FVector ChunkPosition);

	void SetStartingNode(FVector NodePosition);
	void SetNodeAtPosition(FVector NodePosition, ENodeType NodeType);
	void SetNodeBlockingAtPosition(FVector NodePosition, bool isBlocking);

	float GenerateTurnAngle();

	// Read only
	FNodeStruct GetNodeAtPosition(FVector NodePosition);
	TArray<FVector> GetPlayerNodesAtPosition(FVector CenterBottom, FVector FacingDirection, bool onPlatform = false);

	bool IsNodeOpen(FVector NodePosition);

	bool IsStraightPathFree(FVector StartPosition, FVector TravelVector);

	// Returns lowest point for deathbox
	UFUNCTION()
	float GenerateLevel();
	UFUNCTION()
	void RemoveGeneratedLevel();
	UFUNCTION()
	void ClearGrid();

	void SpawnPlatformAtPosition(FVector Position);
		
};
