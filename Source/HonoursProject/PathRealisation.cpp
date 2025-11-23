// Fill out your copyright notice in the Description page of Project Settings.


#include "PathRealisation.h"

#include "RhythmGenerationComponent.h"

// Sets default values for this component's properties
UPathRealisation::UPathRealisation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPathRealisation::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPathRealisation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FPathSection> UPathRealisation::GeneratePathFromRhythmGroup(TArray<FGeneratedBeatValues> RhythmGroup, FVector Origin, float RhythmGroupDuration)
{
	CurrentPath.Empty();

	bool isPathGenerated = false;
	TArray<FGeneratedBeatValues> ValuesToVisit;

	FGeneratedBeatValues currentBeat;

	bool isMoving = false;

	float lowestBeatStartTime;

	while (!isPathGenerated) {
		lowestBeatStartTime = ValuesToVisit[0].StartTime;

		for (FGeneratedBeatValues Beat : ValuesToVisit) {
			if (lowestBeatStartTime > Beat.StartTime) {
				lowestBeatStartTime = Beat.StartTime;
				currentBeat = Beat;
			}
		}

		ValuesToVisit.Remove(currentBeat);

		ExploreCurrentAction(ValuesToVisit, lowestBeatStartTime, currentBeat, isMoving);

		if (!ValuesToVisit.IsEmpty()) {
			continue;
		}

		isPathGenerated = true;
	}

	return TArray<FPathSection>();
}

void UPathRealisation::ExploreCurrentAction(TArray<FGeneratedBeatValues> &ValuesToVisit, float actionExploreStart, 
	FGeneratedBeatValues actionBeingExplored, bool &isMoving)
{
	bool hasFurtherActionBeenExplored;

	switch (actionBeingExplored.ActionType)
	{
	case EActionType::Move:
		isMoving = true;
		break;
	default:
		break;
	}

	float currentActionEndTime = actionExploreStart + actionBeingExplored.Duration;
	float lowestFurtherBeatStartTime = currentActionEndTime;
	FGeneratedBeatValues furtherActionToExplore;

	for (FGeneratedBeatValues Beat : ValuesToVisit) {
		if (lowestFurtherBeatStartTime > Beat.StartTime) {
			lowestFurtherBeatStartTime = Beat.StartTime;
			furtherActionToExplore = Beat;
		}
	}

	if (lowestFurtherBeatStartTime != currentActionEndTime) {
		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:
			
			break;
		default:
			break;
		}
	}
	else {
		FPathSection newSection;

		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:
			newSection.SectionType = EPathSectionType::Flat;
			newSection.st
			break;
		default:
			break;
		}
	}
}

