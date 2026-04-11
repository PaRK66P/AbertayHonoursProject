// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PathRealisation.generated.h"

enum class EActionType : uint8;
enum class ECollectiblePlacementType : uint8;
struct FGeneratedBeatValues;
class UActionGrammarsHolder;
class UEvaluationPCGComponent;

USTRUCT(BlueprintType)
struct FActionEndValues
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EActionType ActionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSloped;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSteep;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsGap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int VerticalDirection; // 0 = Up, 1 = Forward, 2 = Down
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int VerticalSize; // 0 = Short, 1 = Medium, 2 = Long



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
	bool IsMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSloped;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSteep;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsGap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int VerticalDirection; // 0 = Up, 1 = Forward, 2 = Down
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsLargeJump;

	// The distance traveled from the start to end in direction (1, 0, 0)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TravelVector;
	ECollectiblePlacementType collectiblePlacementType;

	bool operator==(const FPathSection& Other) const
	{
		return IsMove == Other.IsMove
			&& IsJump == Other.IsJump
			&& TravelVector == Other.TravelVector;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UPathRealisation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathRealisation();

	void SetActionGrammarReference(UActionGrammarsHolder* reference);
	void SetDataGatheringReference(UEvaluationPCGComponent* reference);
	void StopGatheringData();

	TArray<FPathSection> CurrentPath;
	UActionGrammarsHolder* actionGrammarsReference;
	bool isEvaluating = false;
	UEvaluationPCGComponent* evaluationComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FPathSection> GeneratePathFromRhythmGroup(TArray<FGeneratedBeatValues> RhythmGroup, FVector Origin, float RhythmGroupDuration, FVector StartingDirection);

	void ExploreCurrentAction(TArray<FGeneratedBeatValues> &ValuesToVisit, float actionExploreStart, FGeneratedBeatValues actionBeingExplored, bool &isMoving, TArray<FActionEndValues> & ActionEndList);

	void AddMoveAction(float duration);
	void AddJumpAction();


};
