// Fill out your copyright notice in the Description page of Project Settings.


#include "PathRealisation.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	TArray<FGeneratedBeatValues> ValuesToVisit;

	FGeneratedBeatValues currentBeat;

	bool isMoving = false;

	float lowestBeatStartTime;

	FVector currentPosition = Origin;
	FVector currentDirection = StartingDirection;

	while (!isPathGenerated) {
		lowestBeatStartTime = ValuesToVisit[0].StartTime;

		for (FGeneratedBeatValues Beat : ValuesToVisit) {
			if (lowestBeatStartTime > Beat.StartTime) {
				lowestBeatStartTime = Beat.StartTime;
				currentBeat = Beat;
			}
		}

		ValuesToVisit.Remove(currentBeat);

		ExploreCurrentAction(ValuesToVisit, lowestBeatStartTime, currentBeat, isMoving, currentDirection, currentPosition);

		if (!ValuesToVisit.IsEmpty()) {
			continue;
		}

		isPathGenerated = true;
	}

	return TArray<FPathSection>();
}

void UPathRealisation::ExploreCurrentAction(TArray<FGeneratedBeatValues>& ValuesToVisit, float actionExploreStart, FGeneratedBeatValues actionBeingExplored, bool& isMoving, FVector& CurrentDirection, FVector& CurrentPosition)
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
	FPathSection newSection;

	bool isThereFurtherActionToExplore = true;

	float lastFurtherActionStartTime;

	while (isThereFurtherActionToExplore) {
		isThereFurtherActionToExplore = false;

		for (FGeneratedBeatValues Beat : ValuesToVisit) {
			if (lowestFurtherBeatStartTime > Beat.StartTime) {
				lowestFurtherBeatStartTime = Beat.StartTime;
				furtherActionToExplore = Beat;
				isThereFurtherActionToExplore = true;
			}
		}

		if (!isThereFurtherActionToExplore) {
			break;
		}

		ValuesToVisit.Remove(furtherActionToExplore);

		hasFurtherActionBeenExplored = true;
		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:

			if (lastFurtherActionStartTime != lowestFurtherBeatStartTime) {
				newSection.SectionType = EPathSectionType::Flat;
				newSection.StartPosition = CurrentPosition;
				newSection.EndPosition = GetPositionFromFlatMoving(CurrentPosition, 0.0f, actionBeingExplored.Duration, CurrentDirection);
				CurrentPath.Add(newSection);

				CurrentPosition = newSection.EndPosition;
			}
			ExploreCurrentAction(ValuesToVisit, furtherActionToExplore.StartTime, furtherActionToExplore, isMoving, CurrentDirection, CurrentPosition);

			break;
		default:
			break;
		}

		lastFurtherActionStartTime = lowestFurtherBeatStartTime;
	}

	if (hasFurtherActionBeenExplored) {

	}
	else {
		switch (actionBeingExplored.ActionType)
		{
		case EActionType::Move:
			newSection.SectionType = EPathSectionType::Flat;
			newSection.StartPosition = CurrentPosition;
			newSection.EndPosition = GetPositionFromFlatMoving(CurrentPosition, 0.0f, actionBeingExplored.Duration, CurrentDirection);
			CurrentPath.Add(newSection);

			CurrentPosition = newSection.EndPosition;
			break;
		default:
			break;
		}
	}
}

// Current speed is added for future implementation that might require it
FVector UPathRealisation::GetPositionFromFlatMoving(FVector StartPosition, float CurrentSpeed, float Duration, FVector Direction)
{
	if (Direction.IsNearlyZero()) { return FVector::ZeroVector; }
		
	const UCharacterMovementComponent* movementComponent = PlayerCharacter->GetCharacterMovement();

	float maxSpeed = movementComponent->MaxWalkSpeed;
	float maxAcceleration = movementComponent->GetMaxAcceleration();
	float friction = movementComponent->GroundFriction;
	float braking = movementComponent->BrakingDecelerationWalking;

	const float deltaTime = 0.016f; // 60 FPS
	float velocity = 0.f;
	FVector totalMovement = FVector::ZeroVector;

	FVector normalisedDirection = Direction.GetSafeNormal(); // Ensure normalized

	// Simulate movement frame by frame
	for (float time = 0.f; time < Duration; time += deltaTime)
	{
		velocity += movementComponent->GetMaxAcceleration() * deltaTime;
		velocity = FMath::Min(velocity, movementComponent->MaxWalkSpeed);

		// Apply friction
		velocity *= FMath::Clamp(1.f - movementComponent->GroundFriction * deltaTime, 0.f, 1.f);

		// Move
		totalMovement += normalisedDirection * velocity * deltaTime;
	}

	return StartPosition + totalMovement;
}

