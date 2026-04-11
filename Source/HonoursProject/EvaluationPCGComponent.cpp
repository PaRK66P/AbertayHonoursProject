// Fill out your copyright notice in the Description page of Project Settings.


#include "EvaluationPCGComponent.h"

// Sets default values for this component's properties
UEvaluationPCGComponent::UEvaluationPCGComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEvaluationPCGComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEvaluationPCGComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEvaluationPCGComponent::StartDataGathering()
{
	currentData.Empty();
	currentIndex = -1;
}

void UEvaluationPCGComponent::NextDataSet()
{
	currentData.Add(new FEvaluationValues());
	currentIndex++;
}

// Not necessary I guess
void UEvaluationPCGComponent::ClearEvaluationData()
{
	currentData.Empty();
}

void UEvaluationPCGComponent::CalculateFinalValues()
{
	currentData[currentIndex]->CalculateValues();
}

void UEvaluationPCGComponent::AddPlatformDistance(float distance)
{
	currentData[currentIndex]->totalPlatformDistance += distance;
	currentData[currentIndex]->numberOfPlatforms++;
}

void UEvaluationPCGComponent::AddDifficulty(float difficultyValue)
{
	currentData[currentIndex]->totalDifficulty += difficultyValue;
	currentData[currentIndex]->numberOfDifficultyObjects++;
}

void UEvaluationPCGComponent::AddObjectVolume(float volume)
{
	currentData[currentIndex]->totalObjectVolume += volume;
}

void UEvaluationPCGComponent::SetLevelVolume(float volume)
{
	currentData[currentIndex]->levelVolume = volume;
}

void UEvaluationPCGComponent::AddPatternVarience(int amount)
{
	currentData[currentIndex]->occuredVariencePatterns += amount;
}

void UEvaluationPCGComponent::SetTotalNumberOfPatterns(int amount)
{
	currentData[currentIndex]->totalPossiblePatterns = amount;
}

void UEvaluationPCGComponent::CheckMaxPatternOccurence(int occurences)
{
	currentData[currentIndex]->highestRepeatingPatternAmount = FMath::Max(occurences, currentData[currentIndex]->highestRepeatingPatternAmount);
}

void UEvaluationPCGComponent::AddNumberOfPatternObjects(int amount)
{
	currentData[currentIndex]->numberOfPatternObjects += amount;
}

void UEvaluationPCGComponent::OutputDataToFile()
{
	for (FEvaluationValues* dataSet : currentData) {
		dataSet->CalculateValues();
	}

	UE_LOG(LogTemp, Warning, TEXT("occured %i"), currentData[0]->occuredVariencePatterns);
	UE_LOG(LogTemp, Warning, TEXT("occured %i"), currentData[0]->totalPossiblePatterns);
	UE_LOG(LogTemp, Warning, TEXT("occured %f"), currentData[0]->PatternVarience);

	FDateTime now = FDateTime::Now();

	FString fileName = FString::Printf(
		TEXT("Content/GatheredData/PCGDataValues_%02d_%02d_%04d_%02d_%02d_%02d.csv"),
		now.GetDay(),
		now.GetMonth(),
		now.GetYear(),
		now.GetHour(),
		now.GetMinute(),
		now.GetSecond()
	);

	FString filePath = FPaths::ProjectDir() + fileName;

	FString dataString = "Linearity,Leniency,PlatformDensity,PatternVarience,PatternDensity\n";

	for (FEvaluationValues* dataValue : currentData)
	{
		if (dataValue) // safety check
		{
			dataString += FString::Printf(TEXT("%.4f,%.4f,%.4f,%.4f,%.4f\n"),
				dataValue->Linearity,
				dataValue->Leniency,
				dataValue->PlatformDensity,
				dataValue->PatternVarience,
				dataValue->PatternDensity
			);
		}
	}

	if (FFileHelper::SaveStringToFile(dataString, *filePath)) {
		//UE_LOG(LogTemp, Warning, TEXT("File Saved"));
	}
}

