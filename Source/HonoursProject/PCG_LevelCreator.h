// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PCG_LevelCreator.generated.h"

class UPathRealisation;
class UGeometryRealisationComponent;
class URhythmGenerationComponent;
class UActionGrammarsHolder;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UPCG_LevelCreator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPCG_LevelCreator();

	bool isLoaded;
	bool isDisplayed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Generation")
	int LevelSections;


	UFUNCTION()
	void GenerateLevel(int NumberOfSections);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UPathRealisation* PathRealisation;
	UPROPERTY()
	UGeometryRealisationComponent* GeometryRealisation;
	UPROPERTY()
	URhythmGenerationComponent* RhythmGeneration;
	UPROPERTY()
	UActionGrammarsHolder* ActionGrammarsHolder;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
