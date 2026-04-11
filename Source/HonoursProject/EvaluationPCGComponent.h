// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EvaluationPCGComponent.generated.h"

enum class EPatternTypes : uint8 {
	SteepUpSlope UMETA(DisplayName = "SteepUpSlope"),
	SteepDownSlope UMETA(DisplayName = "SteepDownSlope"),
	StraightPath UMETA(DisplayName = "StraightPath"),
	NormalUpSlope UMETA(DisplayName = "NormalUpSlope"),
	NormalDownSlope UMETA(DisplayName = "NormalDownSlope"),

	LargeUpJump UMETA(DisplayName = "LargeUpJump"),
	LargeForwardJump UMETA(DisplayName = "LargeForwardJump"),
	LargeDownJump UMETA(DisplayName = "LargeDownJump"),
	NormalUpJump UMETA(DisplayName = "NormalUpJump"),
	NormalForwardJump UMETA(DisplayName = "NormalForwardJump"),
	NormalDownJump UMETA(DisplayName = "NormalDownJump"),
	GapLargeUpJump UMETA(DisplayName = "GapLargeUpJump"),
	GapLargeForwardJump UMETA(DisplayName = "GapLargeForwardJump"),
	GapLargeDownJump UMETA(DisplayName = "GapLargeDownJump"),
	GapNormalUpJump UMETA(DisplayName = "GapNormalUpJump"),
	GapNormalForwardJump UMETA(DisplayName = "GapNormalForwardJump"),
	GapNormalDownJump UMETA(DisplayName = "GapNormalDownJump")
};

struct FPatternKey
{
	TArray<int32> Values;

	bool operator==(const FPatternKey& Other) const
	{
		return Values == Other.Values;
	}
};

FORCEINLINE uint32 GetTypeHash(const FPatternKey& Key)
{
	uint32 Hash = 0;

	for (int32 Value : Key.Values)
	{
		Hash = HashCombine(Hash, ::GetTypeHash(Value));
	}

	return Hash;
}

USTRUCT(BlueprintType)
struct FEvaluationValues
{
	GENERATED_USTRUCT_BODY()

public:
	// Level length has been substituted where appropriate (e.g. number of platforms representing level length)
	float Linearity; // Create a line of best fit, sum distance of each platform from line / Number of platforms

	float Leniency; // Approximate difficulty, sum of all difficulty values / number of level objects (can be number of platforms)
	// Difficulty can range from -1 to 1, -1 being safe, 1 being difficult

	float PlatformDensity; // Platforms filling up the space, volume of objects / volume of the level

	float PatternVarience; // The occurence of patterns within the generator (Bit more complex so detailed below)
	/*
	* Use a for loop half the size of the level (round down)
	* Start with pattern length of one (one for each section type)
	* Each pattern type is then checked for occurence
	* Then each pattern type is expanded to the next length however
	* -If the pattern type had one occurence it has not repeated and is not a pattern
	* -If it is not a pattern there is no need to expand it
	* -This will prevent long expansion of the pattern that is equal to the size of the data
	*
	* -Additionally once the second length expansion has been calculated
	* -This can be used for the expansion step
	* -Only if the expansion has zero occurences
	* -Then can ignore that expansion
	* -(e.g. AB = false, BCBDA can ignore BCBDAB expansion)
	*
	*
	* The pattern varience only cares about how many patterns had occured
	* The value will be number of occured patterns / total number of possible repeated patterns
	*/

	float PatternDensity; // Similar to varience, total number of patterns / number of level objects

	//===================================================

	// Value variables
	// Linearity
	float totalPlatformDistance = 0.0f;
	int numberOfPlatforms = 0;
	float platformDistanceMaximum = 10000;

	// Leniency
	float totalDifficulty = 0.0f;
	int numberOfDifficultyObjects = 0;

	// Platform Density
	float totalObjectVolume = 0.0f;
	float levelVolume;

	// Pattern Varience
	int occuredVariencePatterns = 0;
	int totalPossiblePatterns;

	// PatternDensity
	int highestRepeatingPatternAmount = 0;
	int numberOfPatternObjects = 0;

	void CalculateValues() {
		Linearity = FMath::Min(totalPlatformDistance / (float)numberOfPlatforms, platformDistanceMaximum) / platformDistanceMaximum;
		Leniency = ((totalDifficulty / (float)numberOfDifficultyObjects) + 1.0f) / 2.0f;
		PlatformDensity = totalObjectVolume / levelVolume;
		PatternVarience = ((float)occuredVariencePatterns) / ((float)totalPossiblePatterns);
		PatternDensity = ((float)highestRepeatingPatternAmount) / ((float)numberOfPatternObjects);
	}

};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UEvaluationPCGComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEvaluationPCGComponent();

	TArray<FEvaluationValues*> currentData;
	int currentIndex;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartDataGathering();
	void NextDataSet();
	void ClearEvaluationData();

	void CalculateFinalValues();

	void AddPlatformDistance(float distance);
	void AddDifficulty(float difficultyValue);
	void AddObjectVolume(float volume);
	void SetLevelVolume(float volume);
	void AddPatternVarience(int amount);
	void SetTotalNumberOfPatterns(int amount);
	void CheckMaxPatternOccurence(int occurences);
	void AddNumberOfPatternObjects(int amount); // Patterns are sets of objects, so objects will be added in large alongside occurences

	void OutputDataToFile();
};
