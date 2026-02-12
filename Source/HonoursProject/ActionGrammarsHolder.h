// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionGrammarsHolder.generated.h"

enum class ECollectiblePlacementType : uint8 {
	None UMETA(DisplayName = "None"),
	AboveHorizontalPositions UMETA(DisplayName = "Above Horizontal Positions"), // Row above the platforms
	UpStream UMETA(DisplayName = "Up Stream"), // Vertically upwards from start point
	UpArc UMETA(DisplayName = "Up Arc"),
	StraightArc UMETA(DisplayName = "Straight Arc"),
	DownArc UMETA(DisplayName = "Down Arc")
};

USTRUCT(BlueprintType)
struct FTurningValues
{
	GENERATED_USTRUCT_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variance")
	float NoTurn = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variance")
	float SmallTurn = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variance")
	float LargeTurn = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variance", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float ExtendedTurn = 0.2;

	float NoTurnChance;
	float SmallTurnChance;
	float LargeTurnChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
	float SmallTurnAngle = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float LargeTurnAngle = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variance")
	float LeftTurnChance = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variance")
	float RightTurnChance = 50.0f;

	void DetermineChances() {
		float totalChance = NoTurn + SmallTurn + LargeTurn;
		NoTurnChance = NoTurn / totalChance;
		SmallTurnChance = SmallTurn / totalChance;
		LargeTurnChance = LargeTurn / totalChance;
	}
};

USTRUCT(BlueprintType)
struct FPlayerValuesStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 0.8f;
};

USTRUCT(BlueprintType)
struct FPlatformValuesStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SafePlatformScale = FVector(1.0f, 0.2f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RegularPlatformDefaultSize = FVector(1.0f, 800.0f, 1.0f);; // The units a (1.0f, 1.0f, 1.0f) scale would be

};

USTRUCT(BlueprintType)
struct FJumpingValuesStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float GapChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings")
	float ForwardJumpWeighting = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings")
	float UpJumpWeighting = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings")
	float DownJumpWeighting = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float LargeJumpChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float GapDistance = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float UpLargeHeight = 210.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float UpRegularHeight = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float DownLargeHeight = -400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float DownRegularHeight = -200.0f;

};

USTRUCT(BlueprintType)
struct FMovementValuesStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SlopedChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SteepChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weightings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float UpChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float GradualAngleValue = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float SteepAngleValue = 30.0f;
};

USTRUCT(BlueprintType)
struct FCollectableValuesStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CoinGenerationChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
	float CoinSeperationDistance;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UActionGrammarsHolder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionGrammarsHolder();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
	FCollectableValuesStruct CollectableValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
	FTurningValues TurnValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
	FMovementValuesStruct MovementValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
	FJumpingValuesStruct JumpingValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
	FPlayerValuesStruct PlayerValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
	FPlatformValuesStruct PlatformValues;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FCollectableValuesStruct GetCollectableValues() { return CollectableValues; }
	FTurningValues* GetTurnValues() { return &TurnValues; }
	FMovementValuesStruct GetMovementValues() { return MovementValues; }
	FJumpingValuesStruct GetJumpingValues() { return JumpingValues; }
	FPlayerValuesStruct GetPlayersValues() { return PlayerValues; }
	FPlatformValuesStruct GetPlatformValues() { return PlatformValues; }

};
