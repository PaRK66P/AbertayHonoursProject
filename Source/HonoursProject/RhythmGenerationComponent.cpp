// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmGenerationComponent.h"

// Sets default values for this component's properties
URhythmGenerationComponent::URhythmGenerationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URhythmGenerationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void URhythmGenerationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// For now assumes the rhythm type is regular so beats are evenly spaced out
TArray<FGeneratedBeatValues> URhythmGenerationComponent::GenerateRhythmGroup(float Length)
{
	GeneratedRhythmGroup.Empty();

	int numberOfBeats = FMath::FloorToInt(Length / BeatOccurence);
	int numberOfActions = FMath::FloorToInt(numberOfBeats * BeatDensity);
	float actionInterval = Length / numberOfActions;

	// Also running assumption of only two action types
	float totalActionChance = JumpActionWeighting + MoveActionWeighting;

	bool isMoving = false;
	float moveDuration = 0.0f;
	float currentMoveLength = 0.0f;
	bool isJumping = false;
	float jumpDuration = 0.0f;
	float currentJumpLength = 0.0f;

	for (int i = 0; i < Length; i += actionInterval) {

		if (isMoving) {
			moveDuration += actionInterval;
			if (moveDuration >= currentMoveLength) {
				isMoving = false;
			}
		}

		if (isJumping) {
			jumpDuration += actionInterval;
			if (jumpDuration >= currentJumpLength) {
				isJumping = false;
			}
		}

		if (isMoving && isJumping) {
			continue;
		}

		FGeneratedBeatValues newBeat;
		newBeat.StartTime = i;

		bool isJumpAction = FMath::FRandRange(0.0f, totalActionChance) < JumpActionWeighting ? true : false;
		
		if (isMoving) {
			isJumpAction = true;
		}
		if (isJumping) {
			isJumpAction = false;
		}


		newBeat.ActionType = isJumpAction ? EActionType::Jump : EActionType::Move;
		// Jump duration is determined by type of jump
		newBeat.Duration = isJumpAction ? FMath::FRandRange(JumpActionDurationMin, JumpActionDurationMax) : FMath::FRandRange(MoveActionDurationMin, MoveActionDurationMax);

		if (i + newBeat.Duration > Length) {
			newBeat.Duration = Length - i;
		}

		GeneratedRhythmGroup.Add(newBeat);

		if (isJumpAction) {
			isJumping = true;
			jumpDuration = 0.0f;
			currentJumpLength = newBeat.Duration;
		}
		else {
			isMoving = true;
			moveDuration = 0.0f;
			currentMoveLength = newBeat.Duration;
		}
		
	}

	return GeneratedRhythmGroup;
}

