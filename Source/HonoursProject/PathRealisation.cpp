// Fill out your copyright notice in the Description page of Project Settings.


#include "PathRealisation.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "RhythmGenerationComponent.h"
#include "ActionGrammarsHolder.h"

// Sets default values for this component's properties
UPathRealisation::UPathRealisation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPathRealisation::SetActionGrammarReference(UActionGrammarsHolder* reference)
{
	actionGrammarsReference = reference;
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

TArray<FPathSection> UPathRealisation::GeneratePathFromRhythmGroup(TArray<FGeneratedBeatValues> RhythmGroup, FVector Origin, float RhythmGroupDuration, FVector StartingDirection)
{
	CurrentPath.Empty();

	bool isPathGenerated = false;
	TArray<FGeneratedBeatValues> ValuesToVisit = RhythmGroup;

	FGeneratedBeatValues currentBeat;

	bool isMoving = false;

	float lowestBeatStartTime;

	FVector currentPosition = Origin;
	FVector currentDirection = StartingDirection;
	float currentSpeed = 0.0f;

	// Holds when an action ends at what time to be resolved
	TArray<FActionEndValues> actionEndList;

	int indexToRemoveAt = 0;
	

	while (!isPathGenerated) {
		lowestBeatStartTime = ValuesToVisit[0].StartTime;
		currentBeat = ValuesToVisit[0];

		for (int i = 0; i < ValuesToVisit.Num(); i++) {
			if (lowestBeatStartTime > ValuesToVisit[i].StartTime) {
				lowestBeatStartTime = ValuesToVisit[i].StartTime;
				currentBeat = ValuesToVisit[i];
				indexToRemoveAt = i;
			}
		}

		ValuesToVisit.RemoveAt(indexToRemoveAt);

		ExploreCurrentAction(ValuesToVisit, lowestBeatStartTime, currentBeat, isMoving, actionEndList);

		if (!ValuesToVisit.IsEmpty()) {
			continue;
		}

		isPathGenerated = true;
	}

	return CurrentPath;
}

// So the values by address were just one or two at the start
// But at this point there's a lot so maybe consider making them member variables of the class
void UPathRealisation::ExploreCurrentAction(TArray<FGeneratedBeatValues>& ValuesToVisit, float actionExploreStart, FGeneratedBeatValues actionBeingExplored, bool& isMoving, TArray<FActionEndValues>& ActionEndList)
{
	FPathSection newSection;
	bool hasFurtherActionBeenExplored = false;

	switch (actionBeingExplored.ActionType)
	{
	case EActionType::Move:
		isMoving = true;
		break;
	case EActionType::Jump:
		break;
	default:
		break;
	}

	float currentActionEndTime = actionExploreStart + actionBeingExplored.Duration;
	float lowestFurtherBeatStartTime = currentActionEndTime;
	FGeneratedBeatValues furtherActionToExplore;

	bool isThereFurtherActionToExplore = true;
	bool isThereActionToEnd = false;
	bool hasActionEnded = false;

	float lastFurtherActionStartTime;
	float lowestEndActionTime;

	FActionEndValues actionToEnd;

	int indexToRemoveAt;


	// Keeps track of current actions start time between the splits
	// Since it only matters to this iteration it doesn't need to be tracked outside
	float currentStartTime = actionExploreStart;
	// Checks for other actions during the current one
	while (isThereFurtherActionToExplore) {
		isThereFurtherActionToExplore = false;
		lowestFurtherBeatStartTime = currentActionEndTime;

		if (ValuesToVisit.Num() == 0) {
			break;
		}

		for (int i = 0; i < ValuesToVisit.Num(); i++) {
			if (lowestFurtherBeatStartTime > ValuesToVisit[i].StartTime) {
				lowestFurtherBeatStartTime = ValuesToVisit[i].StartTime;
				furtherActionToExplore = ValuesToVisit[i];
				isThereFurtherActionToExplore = true;
				indexToRemoveAt = i;
			}
		}

		// Checking if any actions need to end
		// CHANGE THIS
		if (!ActionEndList.IsEmpty()) {
			// This causes a disrupt in the action, changing the action midway essentially creating a new action
			// Which is why this is then true
			hasFurtherActionBeenExplored = true; 

			lowestEndActionTime = lowestFurtherBeatStartTime;
			for (int i = 0; i < ActionEndList.Num(); i++) {
				if (lowestEndActionTime > ActionEndList[i].EndTime) { // Might be important that the action ends at the same time but it seems minimal so ignore that
					lowestEndActionTime = ActionEndList[i].EndTime;
					actionToEnd = ActionEndList[i];
					indexToRemoveAt = i;
				}
			}
			ActionEndList.RemoveAt(indexToRemoveAt);

			// Resolve rest of the current action before the action ends
			// Not sure this is necessary with new changes, will test now
			switch (actionBeingExplored.ActionType)
			{
			case EActionType::Move:
				break;
			case EActionType::Jump:
				break;
			default:
				break;
			}

			switch (actionToEnd.ActionType)
			{
			case EActionType::Move:
				isMoving = false;
				break;
			case EActionType::Jump:
				break; // Nothing changes
			default:
				break;
			}

			currentStartTime = lowestEndActionTime;
			continue;
		}

		if (!isThereFurtherActionToExplore) {
			break;
		}

		ValuesToVisit.RemoveAt(indexToRemoveAt);

		// Resolve current action up to the start of the further action
		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:
			AddMoveAction(furtherActionToExplore.StartTime - currentStartTime);
			currentStartTime = furtherActionToExplore.StartTime;

			// Check if action ends during the next one
			if (currentActionEndTime < furtherActionToExplore.StartTime + furtherActionToExplore.Duration) {
				FActionEndValues newEndValues;
				newEndValues.ActionType = EActionType::Move;
				newEndValues.EndTime = currentActionEndTime;

				ActionEndList.Add(newEndValues);

				hasActionEnded = true; // Once the further action iterations are finished this action will have ended
				isThereFurtherActionToExplore = false; // This also means we don't need to keep exploring more actions afterwards
			}

			ExploreCurrentAction(ValuesToVisit,
				furtherActionToExplore.StartTime,
				furtherActionToExplore,
				isMoving,
				ActionEndList);

			break;
		case EActionType::Jump:

			// Jumps ignore move actions until finished
			if (currentActionEndTime >= furtherActionToExplore.StartTime + furtherActionToExplore.Duration) {
				break;
			}

			AddJumpAction();
			// Jumps have a fixed time value
			currentStartTime = actionBeingExplored.StartTime + 1.0f;

			hasActionEnded = true;

			switch (furtherActionToExplore.ActionType) {
			case EActionType::Move:
				

				ExploreCurrentAction(ValuesToVisit,
					furtherActionToExplore.StartTime,
					furtherActionToExplore,
					isMoving,
					ActionEndList);

				break;
			default:
				// At the moment jumps can't happen during a jump
				break;
			}

			break;
		default:
			break;
		}

		lastFurtherActionStartTime = lowestFurtherBeatStartTime;
	}


	if (hasActionEnded) {
		return;
	}

	switch (actionBeingExplored.ActionType)
	{
	case EActionType::Move:
		AddMoveAction(actionBeingExplored.Duration - (currentStartTime - actionBeingExplored.StartTime));
		currentStartTime = actionBeingExplored.Duration + actionBeingExplored.StartTime;
		break;
	case EActionType::Jump:
		AddJumpAction();
		// Jumps have a fixed time value
		currentStartTime = actionBeingExplored.StartTime + 1.0f;
		break;
	default:
		break;
	}
}

void UPathRealisation::AddMoveAction(float duration)
{
	FPathSection newSection;
	newSection.IsMove = true;
	newSection.IsJump = false;
	newSection.collectiblePlacementType = ECollectiblePlacementType::AboveHorizontalPositions;

	// Create speed variable somewhere [speed = 5m/s, 1m == 100 Unreal Units]
	FVector distanceVector = FVector(1.0f, 0.0f, 0.0f) * 500.0f * duration;

	float actionSectionChanceTotal = 1.0f;

	newSection.IsSloped = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetMovementValues().SlopedChance;
	if (!(newSection.IsSloped)) {
		newSection.TravelVector = distanceVector;

		CurrentPath.Add(newSection);
	}

	newSection.IsSteep = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetMovementValues().SteepChance;

	newSection.IsUp = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetMovementValues().UpChance;

	// For now hardcoded -> Make variable in ActionGrammarsHolder
	float movementAngle =
		(newSection.IsSteep ?
			actionGrammarsReference->GetMovementValues().SteepAngleValue
			: actionGrammarsReference->GetMovementValues().GradualAngleValue)
		*
		(newSection.IsUp ?
			-1.0f
			: 1.0f);

	newSection.TravelVector = distanceVector.RotateAngleAxis(movementAngle,
		FVector::CrossProduct(FVector::UpVector, distanceVector).GetSafeNormal());

	CurrentPath.Add(newSection);
}

void UPathRealisation::AddJumpAction()
{
	FPathSection newSection;
	newSection.IsJump = true;
	newSection.IsMove = false;

	float actionSectionChanceTotal = 1.0f;

	newSection.IsGap = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetJumpingValues().GapChance;

	actionSectionChanceTotal = actionGrammarsReference->GetJumpingValues().UpJumpWeighting
		+ actionGrammarsReference->GetJumpingValues().DownJumpWeighting
		+ newSection.IsGap ? actionGrammarsReference->GetJumpingValues().ForwardJumpWeighting : 0.0f;

	float actionSectionChanceCurrent = FMath::FRandRange(0.0f, actionSectionChanceTotal);

	if (actionSectionChanceCurrent
		<= actionGrammarsReference->GetJumpingValues().UpJumpWeighting) {
		newSection.VerticalDirection = 0;
	}
	else if (newSection.IsGap
		&& actionSectionChanceCurrent
		<= actionGrammarsReference->GetJumpingValues().UpJumpWeighting
		+ actionGrammarsReference->GetJumpingValues().ForwardJumpWeighting) {
		newSection.VerticalDirection = 1;
	}
	else {
		newSection.VerticalDirection = 2;
	}

	actionSectionChanceTotal = 1.0f;

	newSection.IsLargeJump = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetJumpingValues().LargeJumpChance;

	// Collectible type determination
	if (!newSection.IsGap) {
		newSection.collectiblePlacementType = ECollectiblePlacementType::UpStream;
	}
	else {
		switch (newSection.VerticalDirection)
		{
		case 0:
			newSection.collectiblePlacementType = ECollectiblePlacementType::UpArc;
			break;
		case 1:
			newSection.collectiblePlacementType = ECollectiblePlacementType::StraightArc;
			break;
		case 2:
			newSection.collectiblePlacementType = ECollectiblePlacementType::DownArc;
			break;
		default:
			break;
		}
	}



	FVector distanceVector = FVector::ZeroVector;
	distanceVector += newSection.IsGap ?
		FVector(1.0f, 0.0f, 0.0f) * actionGrammarsReference->GetJumpingValues().GapDistance
		:
		FVector(1.0f, 0.0f, 0.0f) * 100.0f;

	switch (newSection.VerticalDirection) {
	case 0: // Up
		distanceVector.Z += newSection.IsLargeJump ?
			actionGrammarsReference->GetJumpingValues().UpLargeHeight
			: actionGrammarsReference->GetJumpingValues().UpRegularHeight;
		break;
	case 2:
		distanceVector.Z += newSection.IsLargeJump ?
			actionGrammarsReference->GetJumpingValues().DownLargeHeight
			: actionGrammarsReference->GetJumpingValues().DownRegularHeight;
		break;
	default:
		break;
	}

	newSection.TravelVector = distanceVector;

	CurrentPath.Add(newSection);
}

