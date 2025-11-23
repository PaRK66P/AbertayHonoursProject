// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PathRealisation.generated.h"

struct FGeneratedBeatValues;

UENUM(BlueprintType)
enum class EPathSectionType : uint8
{
	Flat UMETA(DisplayName = "Flat"),
	Arc UMETA(DisplayName = "Arc")
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
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UPathRealisation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathRealisation();

	TArray<FPathSection> CurrentPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsSimulation")
	ACharacter* PlayerCharacter;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FPathSection> GeneratePathFromRhythmGroup(TArray<FGeneratedBeatValues> RhythmGroup, FVector Origin, float RhythmGroupDuration);

	void ExploreCurrentAction(TArray<FGeneratedBeatValues> &ValuesToVisit, float actionExploreStart, 
		FGeneratedBeatValues actionBeingExplored, bool &isMoving);
};
