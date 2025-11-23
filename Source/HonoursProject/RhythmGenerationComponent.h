// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RhythmGenerationComponent.generated.h"

UENUM(BlueprintType)
enum class EActionType : uint8
{
	Jump UMETA(DisplayName = "Jump"),
	Move UMETA(DisplayName = "Move")
};

USTRUCT(BlueprintType)
struct FGeneratedBeatValues
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EActionType ActionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StartTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Duration;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API URhythmGenerationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URhythmGenerationComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float BeatDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BeatOccurence;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FGeneratedBeatValues> GeneratedRhythmGroup;

	// Temp - Would be better to have a list of each action type that the dev inputs each params
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpActionWeighting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpActionDurationMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpActionDurationMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveActionWeighting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveActionDurationMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveActionDurationMax;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FGeneratedBeatValues> GenerateRhythmGroup(float Length);

		
};
