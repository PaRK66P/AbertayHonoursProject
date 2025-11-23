// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RhythmGenerationComponent.h"


#include "GeometryRealisationComponent.generated.h"

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
	ENodeType NodeType;

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

	UPROPERTY(/*VisibleAnywhere, BlueprintReadWrite, Category = "Environment"*/)
	TArray<FNodeStruct> ChunkNodes;

	FNodeStruct ErrorNode;

	FChunkStruct() {
		ChunkNodes.SetNum(ChunkSize * ChunkSize);
	}

	FNodeStruct& GetNode(int32 X, int32 Y)
	{
		if (X < 0 || X >= ChunkSize || Y < 0 || Y >= ChunkSize) {

			UE_LOG(LogTemp, Error,
				TEXT("Node out of range: %i, %i"),
				X, Y);
			return ErrorNode;
		}
		return ChunkNodes[X + (Y * ChunkSize)];
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

	UPROPERTY()
	TArray<FChunkStruct> GeometryGrid;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Returns the end position for future extension of the Path
	FVector AddPathToGrid(TArray<FGeneratedBeatValues> Path, FVector PathOrigin);

		
};
