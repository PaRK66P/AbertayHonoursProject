// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGrammarsHolder.h"

// Sets default values for this component's properties
UActionGrammarsHolder::UActionGrammarsHolder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	// Default Grammar setup
	FActionGrammerStruct tempStruct;
	MoveGrammars.Add(EPathSectionType::Move, tempStruct);
	MoveGrammars[EPathSectionType::Move].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Flat, tempStruct);
	MoveGrammars[EPathSectionType::Flat].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped, tempStruct);
	MoveGrammars[EPathSectionType::Sloped].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped_Steep, tempStruct);
	MoveGrammars[EPathSectionType::Sloped_Steep].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped_Gradual, tempStruct);
	MoveGrammars[EPathSectionType::Sloped_Gradual].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped_Steep_Up, tempStruct);
	MoveGrammars[EPathSectionType::Sloped_Steep_Up].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped_Steep_Down, tempStruct);
	MoveGrammars[EPathSectionType::Sloped_Steep_Down].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped_Gradual_Up, tempStruct);
	MoveGrammars[EPathSectionType::Sloped_Gradual_Up].ChanceOfOccuring = 1.0f;
	MoveGrammars.Add(EPathSectionType::Sloped_Gradual_Down, tempStruct);
	MoveGrammars[EPathSectionType::Sloped_Gradual_Down].ChanceOfOccuring = 1.0f;

	JumpGrammars.Add(EPathSectionType::Jump, tempStruct);
	JumpGrammars[EPathSectionType::Jump].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_NoGap, tempStruct);
	JumpGrammars[EPathSectionType::Jump_NoGap].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Forward, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Forward].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Up].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Down].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up_Long, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Up_Long].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up_Medium, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Up_Medium].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up_Short, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Up_Short].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down_Long, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Down_Long].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down_Medium, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Down_Medium].ChanceOfOccuring = 1.0f;
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down_Short, tempStruct);
	JumpGrammars[EPathSectionType::Jump_Gap_Down_Short].ChanceOfOccuring = 1.0f;


}


// Called when the game starts
void UActionGrammarsHolder::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActionGrammarsHolder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UActionGrammarsHolder::GetActionOccurenceChance(EPathSectionType action)
{
	if (MoveGrammars.Contains(action)) {
		return MoveGrammars[action].ChanceOfOccuring;
	}

	if (JumpGrammars.Contains(action)) {
		return JumpGrammars[action].ChanceOfOccuring;
	}

	return 0.0f;
}

// Not all actions have a fixed duration
// Dynamic durations return 0.0f
// Static durations return their values
float UActionGrammarsHolder::GetActionDuration(EPathSectionType action)
{
	switch (action)
	{
	case EPathSectionType::Jump_Gap_Forward:
	case EPathSectionType::Jump_Gap_Up_Long:
	case EPathSectionType::Jump_Gap_Up_Medium:
	case EPathSectionType::Jump_Gap_Up_Short:
	case EPathSectionType::Jump_Gap_Down_Long:
	case EPathSectionType::Jump_Gap_Down_Medium:
	case EPathSectionType::Jump_Gap_Down_Short:
		return JumpGrammars[action].Duration;
	default:
		break;
	}

	return 0.0f;
}

