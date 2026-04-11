// Fill out your copyright notice in the Description page of Project Settings.


#include "PCG_LevelCreator.h"

#include "PathRealisation.h"
#include "GeometryRealisationComponent.h"
#include "RhythmGenerationComponent.h"
#include "ActionGrammarsHolder.h"
#include "EvaluationPCGComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UPCG_LevelCreator::UPCG_LevelCreator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


void UPCG_LevelCreator::GenerateLevel(int NumberOfSections, float RhythmGroupDuration)
{
	//GeometryRealisation->RemoveGeneratedLevel();
	GeometryRealisation->ClearGrid();

	FVector TempOrigin = FVector::Zero(); // Should change but for now default origin is zero

	FVector SectionStartPosition = TempOrigin;
	FVector MessageStartPosition;
	FVector FacingDirection = FVector::ForwardVector;

	//UE_LOG(LogTemp, Warning, TEXT("Started Generating"));

	for (int i = 0; i < NumberOfSections; i++) {
		//UE_LOG(LogTemp, Warning, TEXT("Section %i started"), i);

		TArray<FGeneratedBeatValues> GeneratedRhythmGroup = RhythmGeneration->GenerateRhythmGroup(RhythmGroupDuration);
		if (GeneratedRhythmGroup.IsEmpty()) {
			return;
		}
		//UE_LOG(LogTemp, Log, TEXT("Rhythm group generated"));
		for (FGeneratedBeatValues Beat : GeneratedRhythmGroup) {
			if (Beat.ActionType == EActionType::Move) {
				//UE_LOG(LogTemp, Log, TEXT("Move for %f at %f"), Beat.Duration, Beat.StartTime);
			}
			if (Beat.ActionType == EActionType::Jump) {
				//UE_LOG(LogTemp, Log, TEXT("Jump for %f at %f"), Beat.Duration, Beat.StartTime);
			}
		}

		TArray<FPathSection> pathGenerated = PathRealisation->GeneratePathFromRhythmGroup(GeneratedRhythmGroup, SectionStartPosition, RhythmGroupDuration, FVector::ForwardVector);
		//UE_LOG(LogTemp, Log, TEXT("Path Generated"));
		if (pathGenerated.IsEmpty()) {
			return;
		}

		MessageStartPosition = SectionStartPosition;
		SectionStartPosition = GeometryRealisation->AddPathToGrid(pathGenerated, SectionStartPosition, FacingDirection);
		//UE_LOG(LogTemp, Log, TEXT("Geometry realised"));
		//UE_LOG(LogTemp, Log, TEXT("Geometry from (%f, %f, %f) to (%f, %f, %f)"),
		//	MessageStartPosition.X, MessageStartPosition.Y, MessageStartPosition.Z,
			//SectionStartPosition.X, SectionStartPosition.Y, SectionStartPosition.Z);
	}

	lowestZPosition = fmin(lowestZPosition, GeometryRealisation->GenerateLevel() - 100.0f);
	//UE_LOG(LogTemp, Warning, TEXT("Level Generated"));
	
}

void UPCG_LevelCreator::GenerateLevelData(int NumberOfLevels, int NumberOfSectionsInALevel, float RhythmGroupDuration)
{
	EvaluationComponent->StartDataGathering();
	PathRealisation->SetDataGatheringReference(EvaluationComponent);

	for (int levelIterations = 0; levelIterations < NumberOfLevels; levelIterations++) {

		EvaluationComponent->NextDataSet();

		FVector TempOrigin = FVector::Zero(); // Should change but for now default origin is zero

		FVector SectionStartPosition = TempOrigin;
		FVector FacingDirection = FVector::ForwardVector;

		GeometryRealisation->RemoveGeneratedLevel();
		GeometryRealisation->ClearGrid();

		TArray<FPathSection> levelPathGenerated;

		for (int i = 0; i < NumberOfSectionsInALevel; i++) {

			TArray<FGeneratedBeatValues> GeneratedRhythmGroup = RhythmGeneration->GenerateRhythmGroup(RhythmGroupDuration);
			if (GeneratedRhythmGroup.IsEmpty()) {
				return;
			}

			TArray<FPathSection> pathGenerated = PathRealisation->GeneratePathFromRhythmGroup(GeneratedRhythmGroup, SectionStartPosition, RhythmGroupDuration, FVector::ForwardVector);
			
			if (pathGenerated.IsEmpty()) {

				//UE_LOG(LogTemp, Log, TEXT("Quit"));
				return;
			}

			for (FPathSection& pathSection : pathGenerated) {
				levelPathGenerated.Add(pathSection);
			}


			//UE_LOG(LogTemp, Log, TEXT("Pat obj %f"), pathGenerated.Num());

			SectionStartPosition = GeometryRealisation->AddPathToGrid(pathGenerated, SectionStartPosition, FacingDirection);
		}

		GeometryRealisation->EvaluateGeometry(EvaluationComponent);

		UE_LOG(LogTemp, Warning, TEXT("Path generated: %i"), levelPathGenerated.Num());

		TArray<EPatternTypes> levelPatternsGenerated;

		// Create an array holding the pattern types
		for (FPathSection& pathSection : levelPathGenerated) {
			if (pathSection.IsMove) {
				if (!pathSection.IsSloped) {
					levelPatternsGenerated.Add(EPatternTypes::StraightPath);
					continue;
				}

				if (pathSection.IsSteep) {

					if (pathSection.IsUp) {
						levelPatternsGenerated.Add(EPatternTypes::SteepUpSlope);
						continue;
					}

					levelPatternsGenerated.Add(EPatternTypes::SteepDownSlope);
					continue;
				}

				if (pathSection.IsUp) {
					levelPatternsGenerated.Add(EPatternTypes::NormalUpSlope);
					continue;
				}

				levelPatternsGenerated.Add(EPatternTypes::NormalDownSlope);
				continue;

			}

			if (pathSection.IsGap) {
				if (pathSection.IsLargeJump) {
					switch (pathSection.VerticalDirection)
					{
					case 0:
						levelPatternsGenerated.Add(EPatternTypes::GapLargeUpJump);
						break;
					case 1:
						levelPatternsGenerated.Add(EPatternTypes::GapLargeForwardJump);
						break;
					case 2:
						levelPatternsGenerated.Add(EPatternTypes::GapLargeDownJump);
						break;
					default:
						break;
					}
					continue;
				}

				switch (pathSection.VerticalDirection)
				{
				case 0:
					levelPatternsGenerated.Add(EPatternTypes::GapNormalUpJump);
					break;
				case 1:
					levelPatternsGenerated.Add(EPatternTypes::GapNormalForwardJump);
					break;
				case 2:
					levelPatternsGenerated.Add(EPatternTypes::GapNormalDownJump);
					break;
				default:
					break;
				}
				continue;
			}

			if (pathSection.IsLargeJump) {
				switch (pathSection.VerticalDirection)
				{
				case 0:
					levelPatternsGenerated.Add(EPatternTypes::GapLargeUpJump);
					break;
				case 1:
					levelPatternsGenerated.Add(EPatternTypes::GapLargeForwardJump);
					break;
				case 2:
					levelPatternsGenerated.Add(EPatternTypes::GapLargeDownJump);
					break;
				default:
					break;
				}
				continue;
			}

			switch (pathSection.VerticalDirection)
			{
			case 0:
				levelPatternsGenerated.Add(EPatternTypes::GapNormalUpJump);
				break;
			case 1:
				levelPatternsGenerated.Add(EPatternTypes::GapNormalForwardJump);
				break;
			case 2:
				levelPatternsGenerated.Add(EPatternTypes::GapNormalDownJump);
				break;
			default:
				break;
			}
			continue;
		}


		UE_LOG(LogTemp, Warning, TEXT("Patterns generated: %i"), levelPatternsGenerated.Num());
		EvaluationComponent->AddNumberOfPatternObjects(levelPatternsGenerated.Num());
		
		int maxPatternSize = FMath::FloorToInt(levelPatternsGenerated.Num() / 2.0f);

		int totalPotentialUnqiuePatterns = 0;
		bool expandPatternSize = false;

		int32 highestPatternCount = 0;

		for (int patternSize = 1; patternSize <= maxPatternSize; patternSize++) {
			TMap<FPatternKey, int32> patternTracker;
			expandPatternSize = false;

			for (int startIndex = 0; startIndex <= levelPatternsGenerated.Num() - patternSize; startIndex++) {
				FPatternKey currentPattern;

				for (int i = 0; i < patternSize; i++) {
					currentPattern.Values.Add(static_cast<int32>(levelPatternsGenerated[startIndex + i]));
				}

				int32& patternCount = patternTracker.FindOrAdd(currentPattern);
				patternCount++;

				if (patternCount > 1) {
					expandPatternSize = true;

					highestPatternCount = FMath::Max(patternCount, highestPatternCount);
				}

			}


			UE_LOG(LogTemp, Warning, TEXT("Patterns generated: %i"), levelPatternsGenerated.Num());
			UE_LOG(LogTemp, Warning, TEXT("Pattern Size: %i"), patternSize);
			totalPotentialUnqiuePatterns += levelPatternsGenerated.Num() - patternSize + 1;
			UE_LOG(LogTemp, Warning, TEXT("Unique Patterns: %i"), totalPotentialUnqiuePatterns);


			UE_LOG(LogTemp, Warning, TEXT("Pattern Tracked: %i"), patternTracker.Num());

			UE_LOG(LogTemp, Warning, TEXT("Pattern Count: %i"), highestPatternCount);
			EvaluationComponent->AddPatternVarience(patternTracker.Num());
			EvaluationComponent->CheckMaxPatternOccurence(highestPatternCount);

			if (!expandPatternSize) {
				break;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Unique Patterns: %i"), totalPotentialUnqiuePatterns);
		EvaluationComponent->SetTotalNumberOfPatterns(totalPotentialUnqiuePatterns);


		
	}
	

	EvaluationComponent->OutputDataToFile();
}



// Called when the game starts
void UPCG_LevelCreator::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("Begin"));

	PathRealisation = GetOwner()->FindComponentByClass<UPathRealisation>();
	if (!PathRealisation) {
		//UE_LOG(LogTemp, Warning, TEXT("No Path"));
	}
	GeometryRealisation = GetOwner()->FindComponentByClass<UGeometryRealisationComponent>();
	if (!GeometryRealisation) {
		//UE_LOG(LogTemp, Warning, TEXT("No Geometry"));
	}
	RhythmGeneration = GetOwner()->FindComponentByClass<URhythmGenerationComponent>();
	if (!RhythmGeneration) {
		//UE_LOG(LogTemp, Warning, TEXT("No Rhythm"));
	}
	ActionGrammarsHolder = GetOwner()->FindComponentByClass<UActionGrammarsHolder>();
	if (!ActionGrammarsHolder) {
		//UE_LOG(LogTemp, Warning, TEXT("No Action Grammars"));
	}
	EvaluationComponent = GetOwner()->FindComponentByClass<UEvaluationPCGComponent>();
	if (!EvaluationComponent) {
		//UE_LOG(LogTemp, Warning, TEXT("No Evaluation Component"));
	}


	if (!PathRealisation || !GeometryRealisation || !RhythmGeneration || !ActionGrammarsHolder || !EvaluationComponent) {
		return;
	}

	PathRealisation->SetActionGrammarReference(ActionGrammarsHolder);
	GeometryRealisation->InitialiseComponent(ActionGrammarsHolder);

	if (GenerateDataSet) {
		GenerateLevelData(NumberOfDataSets, LevelSections, 20.0f);
	}

	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);
	PlayerStart += FVector::UpVector * 100;
	GenerateLevel(LevelSections, 20.0f);


}


// Called every frame
void UPCG_LevelCreator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetActorLocation().Z < lowestZPosition) {
			PlayerCharacter->SetActorLocation(PlayerStart + PlayerOffset);
		}
	}
}

