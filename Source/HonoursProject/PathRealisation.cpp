// Fill out your copyright notice in the Description page of Project Settings.


#include "PathRealisation.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	FPathSection startSection;
	startSection.SectionType = EPathSectionType::Safe;
	startSection.StartPosition = Origin;
	startSection.EndPosition = Origin;
	CurrentPath.Add(startSection);

	int indexToRemoveAt = 0;
	currentBeat = ValuesToVisit[0];

	while (!isPathGenerated) {
		lowestBeatStartTime = ValuesToVisit[0].StartTime;

		for (int i = 0; i < ValuesToVisit.Num(); i++) {
			if (lowestBeatStartTime > ValuesToVisit[i].StartTime) {
				lowestBeatStartTime = ValuesToVisit[i].StartTime;
				currentBeat = ValuesToVisit[i];
				indexToRemoveAt = i;
			}
		}

		ValuesToVisit.RemoveAt(indexToRemoveAt);

		ExploreCurrentAction(ValuesToVisit, lowestBeatStartTime, currentBeat, isMoving, currentDirection, currentPosition, currentSpeed, actionEndList);

		if (!ValuesToVisit.IsEmpty()) {
			continue;
		}

		isPathGenerated = true;
	}

	return CurrentPath;
}

// So the values by address were just one or two at the start
// But at this point there's a lot so maybe consider making them member variables of the class
void UPathRealisation::ExploreCurrentAction(TArray<FGeneratedBeatValues>& ValuesToVisit, float actionExploreStart, FGeneratedBeatValues actionBeingExplored, bool& isMoving, FVector& CurrentDirection, FVector& CurrentPosition, float& CurrentSpeed, TArray<FActionEndValues>& ActionEndList)
{
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
	FPathSection newSection;

	FVector currentActionTrueStartPosition = CurrentPosition;

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
			switch (actionBeingExplored.ActionType)
			{
			case EActionType::Move:
				// Split the action and resolve the part before the further action takes place
				newSection.SectionType = EPathSectionType::Flat;
				newSection.StartPosition = CurrentPosition;
				newSection.EndPosition = GetPositionFromFlatMoving(
					CurrentPosition, 
					CurrentSpeed, 
					lowestEndActionTime - currentStartTime,
					CurrentDirection);
				CurrentPath.Add(newSection);

				CurrentPosition = newSection.EndPosition;

				// Ends of movements are always a safe spot
				newSection.SectionType = EPathSectionType::Safe;
				newSection.StartPosition = CurrentPosition;
				newSection.EndPosition = CurrentPosition;
				CurrentPath.Add(newSection);
				break;
			case EActionType::Jump:
				newSection.SectionType = EPathSectionType::IncompleteArc;
				newSection.StartPosition = CurrentPosition;
				newSection.EndPosition = DetermineArcEndpoint(
					currentActionTrueStartPosition,
					lowestEndActionTime - actionBeingExplored.StartTime,
					CurrentSpeed,
					CurrentDirection);
				CurrentPath.Add(newSection);

				CurrentPosition = newSection.EndPosition;
				break;
			default:
				break;
			}

			switch (actionToEnd.ActionType)
			{
			case EActionType::Move:
				isMoving = false;
				CurrentSpeed = 0.0f;
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

		hasFurtherActionBeenExplored = true;
		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:
			// Split the action and resolve the part before the further action takes place
			newSection.SectionType = EPathSectionType::Flat;
			newSection.StartPosition = CurrentPosition;
			newSection.EndPosition = GetPositionFromFlatMoving(
				CurrentPosition, 
				CurrentSpeed, 
				furtherActionToExplore.StartTime - currentStartTime,
				CurrentDirection);
			CurrentPath.Add(newSection);

			CurrentPosition = newSection.EndPosition;

			// Ends of movements are always a safe spot
			newSection.SectionType = EPathSectionType::Safe;
			newSection.StartPosition = CurrentPosition;
			newSection.EndPosition = CurrentPosition;
			CurrentPath.Add(newSection);

			currentStartTime = lowestFurtherBeatStartTime + furtherActionToExplore.Duration;

			if (furtherActionToExplore.Duration + furtherActionToExplore.StartTime > actionBeingExplored.StartTime + actionBeingExplored.Duration) {
				FActionEndValues newEndValues;
				newEndValues.ActionType = EActionType::Move;
				newEndValues.EndTime = actionBeingExplored.StartTime + actionBeingExplored.Duration;
				
				ActionEndList.Add(newEndValues);

				hasActionEnded = true; // Once the further action iterations are finished this action will have ended
				isThereFurtherActionToExplore = false; // This also means we don't need to keep exploring more actions afterwards
			}

			ExploreCurrentAction(ValuesToVisit, 
				furtherActionToExplore.StartTime, 
				furtherActionToExplore, 
				isMoving, CurrentDirection, 
				CurrentPosition, 
				CurrentSpeed, 
				ActionEndList);

			break;
		// With current implementation this arguable does nothing. The movement is lateral and doesn't interrupt the jump arc and jumping can't happen twice.
		// However later implementation will have movement interupt the arc horizontal movement with turning.
		// Thus the current implementation.
		case EActionType::Jump:
			newSection.SectionType = EPathSectionType::IncompleteArc;
			newSection.StartPosition = CurrentPosition;
			newSection.EndPosition = DetermineArcEndpoint(
				currentActionTrueStartPosition,
				furtherActionToExplore.StartTime - actionBeingExplored.StartTime,
				CurrentSpeed, 
				CurrentDirection);
			CurrentPath.Add(newSection);

			CurrentPosition = newSection.EndPosition;

			currentStartTime = lowestFurtherBeatStartTime;

			if (furtherActionToExplore.ActionType == EActionType::Move) {
				// We can't move during a jump but it does change if the jump is in motion or not
				// Because of this we have to handle this case uniquely
				isMoving = true;

				if (furtherActionToExplore.Duration + furtherActionToExplore.StartTime >
					actionBeingExplored.Duration + actionBeingExplored.StartTime) {
					// This situation becomes very funky and more complex to handle as the action needs to track a path once this action is done
					// The simple hacky way to do this is to add another action to the rhythm group
					// This action is for the tracking part at the end
					// It won't overlap with the action as it has already been removed from the list
					FGeneratedBeatValues tempMoveBeat;
					tempMoveBeat.ActionType = EActionType::Move;
					tempMoveBeat.StartTime = actionBeingExplored.Duration + actionBeingExplored.StartTime;
					tempMoveBeat.Duration = furtherActionToExplore.Duration;

					ValuesToVisit.Add(tempMoveBeat);
				}
				else {
					FActionEndValues newEndValues;
					newEndValues.ActionType = EActionType::Move;
					newEndValues.EndTime = actionBeingExplored.StartTime + actionBeingExplored.Duration;

					ActionEndList.Add(newEndValues);
				}
				break; // We don't want to explore this action as the path would be added to the current jump happening
			}

			ExploreCurrentAction(ValuesToVisit, 
				furtherActionToExplore.StartTime, 
				furtherActionToExplore, 
				isMoving, 
				CurrentDirection, 
				CurrentPosition, 
				CurrentSpeed, 
				ActionEndList);
			break;
		default:
			break;
		}

		lastFurtherActionStartTime = lowestFurtherBeatStartTime;
	}

	switch (actionBeingExplored.ActionType)
	{
	case EActionType::Move:
		newSection.SectionType = EPathSectionType::Flat;
		newSection.StartPosition = CurrentPosition;
		newSection.EndPosition = GetPositionFromFlatMoving(
			CurrentPosition, 
			CurrentSpeed, 
			actionBeingExplored.Duration - (currentStartTime - actionBeingExplored.StartTime), 
			CurrentDirection);
		CurrentPath.Add(newSection);

		CurrentPosition = newSection.EndPosition;

		isMoving = false;
		CurrentSpeed = 0.0f;
		break;
	case EActionType::Jump:
		newSection.SectionType = hasFurtherActionBeenExplored ? 
			EPathSectionType::IncompleteArc : EPathSectionType::Arc;
		newSection.StartPosition = CurrentPosition;

		newSection.EndPosition = DetermineArcEndpoint(
			(hasFurtherActionBeenExplored ? 
				currentActionTrueStartPosition : CurrentPosition), // Arc vertical position will never be changed with actions so we can calculate the whole arc from the start even if it was split up
			actionBeingExplored.Duration, // Duration doesn't change as we calculate the full arc
			CurrentSpeed,
			CurrentDirection);
		CurrentPath.Add(newSection);

		CurrentPosition = newSection.EndPosition;
		break;
	default:
		break;
	}

	// Signal in the path that if the platforms were unclear one would guarantee to be here
	newSection.SectionType = EPathSectionType::Safe;
	newSection.StartPosition = CurrentPosition;
	newSection.EndPosition = CurrentPosition;
	CurrentPath.Add(newSection);
}

FVector UPathRealisation::DetermineArcEndpoint(FVector StartPosition, float Duration, float CurrentSpeed, FVector CurrentDirection)
{
	const UCharacterMovementComponent* moveComponent = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetCharacterMovement();

	FVector arcPosition = StartPosition;

	const float gravityZ = moveComponent->GetGravityZ();

	arcPosition += CurrentDirection * CurrentSpeed * Duration;

	// Ballistic arc formula for vertical movement
	float verticalDisplacement = (moveComponent->JumpZVelocity * Duration) +
		(0.5f * gravityZ * Duration * Duration);

	arcPosition.Z += verticalDisplacement;

	return arcPosition;
}

// Current speed is added for future implementation that might require it (IT WAS NEEDED :D)
FVector UPathRealisation::GetPositionFromFlatMoving(FVector StartPosition, float& CurrentSpeed, float Duration, FVector Direction)
{
	if (Direction.IsNearlyZero()) { return FVector::ZeroVector; }
		
	const UCharacterMovementComponent* movementComponent = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetCharacterMovement();

	float maxSpeed = movementComponent->MaxWalkSpeed;
	float maxAcceleration = movementComponent->GetMaxAcceleration();
	float friction = movementComponent->GroundFriction;
	float braking = movementComponent->BrakingDecelerationWalking;

	const float deltaTime = 0.016f; // 60 FPS
	FVector totalMovement = FVector::ZeroVector;

	FVector normalisedDirection = Direction.GetSafeNormal(); // Ensure normalized

	// Simulate movement frame by frame
	for (float time = 0.f; time < Duration; time += deltaTime)
	{
		CurrentSpeed += movementComponent->GetMaxAcceleration() * deltaTime;
		CurrentSpeed = FMath::Min(CurrentSpeed, movementComponent->MaxWalkSpeed);

		// Apply friction
		CurrentSpeed *= FMath::Clamp(1.f - movementComponent->GroundFriction * deltaTime, 0.f, 1.f);

		// Move
		totalMovement += normalisedDirection * CurrentSpeed * deltaTime;
	}

	return StartPosition + totalMovement;
}

