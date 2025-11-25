// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PathRealisation.generated.h"

enum class EActionType : uint8;
struct FGeneratedBeatValues;

UENUM(BlueprintType)
enum class EPathSectionType : uint8
{
	Flat UMETA(DisplayName = "Flat"),
	Arc UMETA(DisplayName = "Arc"),
	Safe UMETA(DisplayName = "Safe"), // Refers to a positions that guarantees they are on a platform as all points are not always guaranteed
	IncompleteArc UMETA(DisplayName = "Incomplete Arc") // Not all jump arcs are full as they will be split up through different points
};

USTRUCT(BlueprintType)
struct FActionEndValues
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EActionType ActionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float EndTime;

	bool operator==(const FActionEndValues& Other) const
	{
		return ActionType == Other.ActionType
			&& EndTime == Other.EndTime;
	}
};

USTRUCT(BlueprintType)
struct FPathSection
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPathSectionType SectionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector StartPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector EndPosition;

	bool operator==(const FPathSection& Other) const
	{
		return SectionType == Other.SectionType
			&& StartPosition == Other.StartPosition
			&& EndPosition == Other.EndPosition;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UPathRealisation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathRealisation();

	TArray<FPathSection> CurrentPath;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FPathSection> GeneratePathFromRhythmGroup(TArray<FGeneratedBeatValues> RhythmGroup, FVector Origin, float RhythmGroupDuration, FVector StartingDirection);

	void ExploreCurrentAction(TArray<FGeneratedBeatValues> &ValuesToVisit, float actionExploreStart, FGeneratedBeatValues actionBeingExplored, bool &isMoving, FVector & CurrentDirection, FVector & CurrentPosition, float & CurrentSpeed, TArray<FActionEndValues> & ActionEndList);

	FVector DetermineArcEndpoint(FVector StartPosition, float Duration, float CurrentSpeed, FVector CurrentDirection);

	// Physics Calculations
	FVector GetPositionFromFlatMoving(FVector StartPosition, float & CurrentSpeed, float Duration, FVector Direction);

};
