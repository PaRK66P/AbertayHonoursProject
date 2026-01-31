// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionGrammarsHolder.generated.h"

// In future can set enums as flags
// e.g. gradual/steep, up/down, rather than four individual catagories
UENUM(BlueprintType)
enum class EPathSectionType : uint8
{
	// Move Action Grammars
	Move UMETA(DisplayName = "Move"),
	Flat UMETA(DisplayName = "Flat"),
	Sloped UMETA(DisplayName = "Sloped"),
	Sloped_Steep UMETA(DisplayName = "Steep"),
	Sloped_Gradual UMETA(DisplayName = "Gradual"),
	Sloped_Steep_Up UMETA(DisplayName = "Steep Up"),
	Sloped_Steep_Down UMETA(DisplayName = "Steep Down"),
	Sloped_Gradual_Up UMETA(DisplayName = "Gradual Up"),
	Sloped_Gradual_Down UMETA(DisplayName = "Gradual Down"),
	// Jumps
	Jump UMETA(DisplayName = "Jump"),
	Jump_Gap UMETA(DisplayName = "Gap"),
	Jump_NoGap UMETA(DisplayName = "No Gap"),
	Jump_Gap_Forward UMETA(DisplayName = "Gap Forward"),
	Jump_Gap_Up UMETA(DisplayName = "Gap Up"),
	Jump_Gap_Down UMETA(DisplayName = "Gap Down"),
	Jump_NoGap_Up UMETA(DisplayName = "No Gap"),
	Jump_NoGap_Down UMETA(DisplayName = "No Gap"),
	Jump_NoGap_Up_Long UMETA(DisplayName = "No Gap Up Long"),
	Jump_NoGap_Up_Medium UMETA(DisplayName = "No Gap Up Medium"),
	Jump_NoGap_Up_Short UMETA(DisplayName = "No Gap Up Short"),
	Jump_NoGap_Down_Long UMETA(DisplayName = "No Gap Down Long"),
	Jump_NoGap_Down_Medium UMETA(DisplayName = "No Gap Down Medium"),
	Jump_NoGap_Down_Short UMETA(DisplayName = "No Gap Down Short"),
	Jump_Gap_Up_Long UMETA(DisplayName = "Gap Up Long"),
	Jump_Gap_Up_Medium UMETA(DisplayName = "Gap Up Medium"),
	Jump_Gap_Up_Short UMETA(DisplayName = "Gap Up Short"),
	Jump_Gap_Down_Long UMETA(DisplayName = "Gap Down Long"),
	Jump_Gap_Down_Medium UMETA(DisplayName = "Gap Down Medium"),
	Jump_Gap_Down_Short UMETA(DisplayName = "Gap Down Short"),

	Safe UMETA(DisplayName = "Safe"), // Refers to a positions that guarantees they are on a platform as all points are not always guaranteed
};

USTRUCT(BlueprintType)
struct FActionGrammerStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float ChanceOfOccuring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float Duration;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONOURSPROJECT_API UActionGrammarsHolder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionGrammarsHolder();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapForwardDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapUpLongDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapUpMediumDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapUpShortDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapDownLongDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapDownMediumDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	float JumpGapDownShortDuration;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	TMap<EPathSectionType, FActionGrammerStruct> MoveGrammars;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grammars")
	TMap<EPathSectionType, FActionGrammerStruct> JumpGrammars;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetActionOccurenceChance(EPathSectionType action);
	float GetActionDuration(EPathSectionType action);


};
