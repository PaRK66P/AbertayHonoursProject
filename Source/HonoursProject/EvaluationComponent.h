// Fill out your copyright notice in the Description page of Project Settings.


/*
* SO TURNS OUT THERE IS SOME NAMING CONVENTION ISSUES WITH THIS AS UNREAL ALREADY HAS AN EVALUATION COMPONENT
* THIS IS A DEAD CLASS BASICALLY I WILL WANT TO DELETE
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EvaluationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UEvaluationComponent : public UActorComponent
{
	GENERATED_BODY()



public:	
	// Sets default values for this component's properties
	UEvaluationComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
