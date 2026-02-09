// Fill out your copyright notice in the Description page of Project Settings.


#include "PCG_LevelCreator.h"

#include "PathRealisation.h"
#include "GeometryRealisationComponent.h"
#include "RhythmGenerationComponent.h"
#include "ActionGrammarsHolder.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UPCG_LevelCreator::UPCG_LevelCreator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


void UPCG_LevelCreator::GenerateLevel(int NumberOfSections)
{
	FVector TempOrigin = FVector::Zero(); // Should change but for now default origin is zero

	FVector SectionStartPosition = TempOrigin;
	FVector MessageStartPosition;
	FVector FacingDirection = FVector::ForwardVector;

	UE_LOG(LogTemp, Warning, TEXT("Started Generating"));

	for (int i = 0; i < NumberOfSections; i++) {
		UE_LOG(LogTemp, Warning, TEXT("Section %i started"), i);

		TArray<FGeneratedBeatValues> GeneratedRhythmGroup = RhythmGeneration->GenerateRhythmGroup(20);
		if (GeneratedRhythmGroup.IsEmpty()) {
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Rhythm group generated"));
		for (FGeneratedBeatValues Beat : GeneratedRhythmGroup) {
			if (Beat.ActionType == EActionType::Move) {
				UE_LOG(LogTemp, Log, TEXT("Move for %f at %f"), Beat.Duration, Beat.StartTime);
			}
			if (Beat.ActionType == EActionType::Jump) {
				UE_LOG(LogTemp, Log, TEXT("Jump for %f at %f"), Beat.Duration, Beat.StartTime);
			}
		}

		TArray<FPathSection> pathGenerated = PathRealisation->GeneratePathFromRhythmGroup(GeneratedRhythmGroup, SectionStartPosition, 20, FVector::ForwardVector);
		UE_LOG(LogTemp, Log, TEXT("Path Generated"));
		if (pathGenerated.IsEmpty()) {
			return;
		}

		MessageStartPosition = SectionStartPosition;
		SectionStartPosition = GeometryRealisation->AddPathToGrid(pathGenerated, SectionStartPosition, FacingDirection);
		UE_LOG(LogTemp, Log, TEXT("Geometry realised"));
		UE_LOG(LogTemp, Log, TEXT("Geometry from (%f, %f, %f) to (%f, %f, %f)"),
			MessageStartPosition.X, MessageStartPosition.Y, MessageStartPosition.Z,
			SectionStartPosition.X, SectionStartPosition.Y, SectionStartPosition.Z);
	}

	lowestZPosition = fmin(lowestZPosition, GeometryRealisation->GenerateLevel() - 100.0f);
	UE_LOG(LogTemp, Warning, TEXT("Level Generated"));
	
}

// Called when the game starts
void UPCG_LevelCreator::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Begin"));

	PathRealisation = GetOwner()->FindComponentByClass<UPathRealisation>();
	if (!PathRealisation) {
		UE_LOG(LogTemp, Warning, TEXT("No Path"));
	}
	GeometryRealisation = GetOwner()->FindComponentByClass<UGeometryRealisationComponent>();
	if (!GeometryRealisation) {
		UE_LOG(LogTemp, Warning, TEXT("No Geometry"));
	}
	RhythmGeneration = GetOwner()->FindComponentByClass<URhythmGenerationComponent>();
	if (!RhythmGeneration) {
		UE_LOG(LogTemp, Warning, TEXT("No Rhythm"));
	}
	ActionGrammarsHolder = GetOwner()->FindComponentByClass<UActionGrammarsHolder>();
	if (!ActionGrammarsHolder) {
		UE_LOG(LogTemp, Warning, TEXT("No Action Grammars"));
	}

	if (!PathRealisation || !GeometryRealisation || !RhythmGeneration || !ActionGrammarsHolder) {
		return;
	}

	PathRealisation->SetActionGrammarReference(ActionGrammarsHolder);
	GeometryRealisation->InitialiseComponent(ActionGrammarsHolder);

	GenerateLevel(LevelSections);

}


// Called every frame
void UPCG_LevelCreator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetActorLocation().Z < lowestZPosition) {
			PlayerCharacter->SetActorLocation(PlayerStart);
		}
	}
}

