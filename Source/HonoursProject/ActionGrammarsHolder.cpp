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
	MoveGrammars.Add(EPathSectionType::Flat, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped_Steep, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped_Gradual, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped_Steep_Up, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped_Steep_Down, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped_Gradual_Up, tempStruct);
	MoveGrammars.Add(EPathSectionType::Sloped_Gradual_Down, tempStruct);

	JumpGrammars.Add(EPathSectionType::Jump, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_NoGap, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Forward, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up_Long, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up_Medium, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Up_Short, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down_Long, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down_Medium, tempStruct);
	JumpGrammars.Add(EPathSectionType::Jump_Gap_Down_Short, tempStruct);

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

