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

		// Resolve current action up to the start of the further action
		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:

			UE_LOG(LogTemp, Log, TEXT("Move action %f %f %f"),
				furtherActionToExplore.StartTime, currentStartTime, actionBeingExplored.StartTime);
			CurrentPosition = AddMoveAction(CurrentPosition, CurrentDirection,
				furtherActionToExplore.StartTime - currentStartTime);
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
				isMoving, CurrentDirection,
				CurrentPosition,
				CurrentSpeed,
				ActionEndList);

			break;
		case EActionType::Jump:

			// Jumps ignore move actions until finished
			if (currentActionEndTime >= furtherActionToExplore.StartTime + furtherActionToExplore.Duration) {
				break;
			}

			CurrentPosition = AddJumpAction(CurrentPosition, CurrentDirection);
			// Jumps have a fixed time value
			currentStartTime = actionBeingExplored.StartTime + 1.0f;

			hasActionEnded = true;

			switch (furtherActionToExplore.ActionType) {
			case EActionType::Move:
				

				ExploreCurrentAction(ValuesToVisit,
					furtherActionToExplore.StartTime,
					furtherActionToExplore,
					isMoving, CurrentDirection,
					CurrentPosition,
					CurrentSpeed,
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
		UE_LOG(LogTemp, Log, TEXT("Move action %f %f %f"),
			actionBeingExplored.Duration, currentStartTime, actionBeingExplored.StartTime);
		CurrentPosition = AddMoveAction(CurrentPosition, CurrentDirection,
			actionBeingExplored.Duration - (currentStartTime - actionBeingExplored.StartTime));
		currentStartTime = actionBeingExplored.Duration + actionBeingExplored.StartTime;
		break;
	case EActionType::Jump:
		CurrentPosition = AddJumpAction(CurrentPosition, CurrentDirection);
		// Jumps have a fixed time value
		currentStartTime = actionBeingExplored.StartTime + 1.0f;
		break;
	default:
		break;
	}
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

// This was not needed (D:)
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

FVector UPathRealisation::AddMoveAction(FVector startingPosition, FVector facingDirection, float duration)
{

	UE_LOG(LogTemp, Log, TEXT("Facing %f %f %f"),
		facingDirection.X, facingDirection.Y, facingDirection.Z);
	FPathSection newSection;
	newSection.SectionType = EPathSectionType::Move;
	newSection.IsMove = true;
	newSection.StartPosition = startingPosition;

	// Create speed variable somewhere [speed = 5m/s, 1m == 100 Unreal Units]
	FVector distanceVector = 500 * facingDirection * duration;

	float actionSectionChanceTotal = actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Flat)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped);

	newSection.IsSloped = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Flat);
	if (!(newSection.IsSloped)) {
		newSection.EndPosition = distanceVector + startingPosition;

		CurrentPath.Add(newSection);
		UE_LOG(LogTemp, Log, TEXT("Moving from (%f, %f, %f) to (%f, %f, %f)"),
			startingPosition.X, startingPosition.Y, startingPosition.Z,
			newSection.EndPosition.X, newSection.EndPosition.Y, newSection.EndPosition.Z);

		return newSection.EndPosition;
	}

	actionSectionChanceTotal = actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Steep)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Gradual);

	newSection.IsSteep = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Steep);


	actionSectionChanceTotal = newSection.IsSteep ?
		actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Steep_Up)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Steep_Down)
		: actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Gradual_Up)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Sloped_Gradual_Down);

	newSection.IsUp = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetActionOccurenceChance(newSection.IsSteep ?
			EPathSectionType::Sloped_Steep_Up
			: EPathSectionType::Sloped_Gradual_Up);

	// For now hardcoded -> Make variable in ActionGrammarsHolder
	float movementAngle =
		(newSection.IsSteep ?
			30.0f
			: 15.0f)
		*
		(newSection.IsUp ?
			-1.0f
			: 1.0f);

	newSection.EndPosition = distanceVector.RotateAngleAxis(movementAngle,
		FVector::CrossProduct(FVector::UpVector, distanceVector).GetSafeNormal())
		+ startingPosition;

	CurrentPath.Add(newSection);

	UE_LOG(LogTemp, Log, TEXT("Moving from (%f, %f, %f) to (%f, %f, %f)"),
		startingPosition.X, startingPosition.Y, startingPosition.Z,
		newSection.EndPosition.X, newSection.EndPosition.Y, newSection.EndPosition.Z);

	return newSection.EndPosition;
}

FVector UPathRealisation::AddJumpAction(FVector startingPosition, FVector facingDirection)
{
	UE_LOG(LogTemp, Log, TEXT("Facing %f %f %f"),
		facingDirection.X, facingDirection.Y, facingDirection.Z);

	FPathSection newSection;
	newSection.SectionType = EPathSectionType::Jump;
	newSection.IsJump = true;
	newSection.StartPosition = startingPosition;

	float actionSectionChanceTotal = actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_NoGap);

	newSection.IsGap = FMath::FRandRange(0.0f, actionSectionChanceTotal)
		<= actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap);

	actionSectionChanceTotal =
		newSection.IsGap ?
		actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap_Up)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap_Forward)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap_Down)
		: actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_NoGap_Up)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_NoGap_Down);


	float actionSectionChanceCurrent = FMath::FRandRange(0.0f, actionSectionChanceTotal);

	if (actionSectionChanceCurrent
		<= actionGrammarsReference->GetActionOccurenceChance(
			newSection.IsGap ?
			EPathSectionType::Jump_Gap_Up
			: EPathSectionType::Jump_NoGap_Up)) {
		newSection.VerticalDirection = 0;
	}
	else if (newSection.IsGap
		&& actionSectionChanceCurrent
		<= actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap_Up)
		+ actionGrammarsReference->GetActionOccurenceChance(EPathSectionType::Jump_Gap_Forward)) {
		newSection.VerticalDirection = 1;
	}
	else {
		newSection.VerticalDirection = 2;
	}

	// Why is this section hardcoded?
	// Because I didn't want a triple indented 3 split per indent if statement web that I couldn't understand
	// PLEASE GET THE BIT FLAGS IMPLEMENTED
	actionSectionChanceTotal = 0.0f;
	newSection.VerticalSize = 1; // Fixed for now
	FVector distanceVector = FVector::ZeroVector;
	distanceVector += newSection.IsGap ?
		facingDirection * 400.0f
		:
		facingDirection * 100.0f;
	switch (newSection.VerticalDirection) {
	case 0: // Up
		distanceVector.Z += 200.0f;
		break;
	case 2:
		distanceVector.Z -= 200.0f;
		break;
	default:
		break;
	}

	newSection.EndPosition = startingPosition + distanceVector;

	CurrentPath.Add(newSection);

	UE_LOG(LogTemp, Log, TEXT("Jumping from (%f, %f, %f) to (%f, %f, %f)"), 
		startingPosition.X, startingPosition.Y, startingPosition.Z,
		newSection.EndPosition.X, newSection.EndPosition.Y, newSection.EndPosition.Z);

	return newSection.EndPosition;
}

