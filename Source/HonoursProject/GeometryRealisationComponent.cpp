// Fill out your copyright notice in the Description page of Project Settings.


#include "GeometryRealisationComponent.h"

#include "PathRealisation.h"
#include "ActionGrammarsHolder.h"
#include "EvaluationPCGComponent.h"

// Sets default values for this component's properties
UGeometryRealisationComponent::UGeometryRealisationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGeometryRealisationComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGeometryRealisationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGeometryRealisationComponent::InitialiseComponent(UActionGrammarsHolder* ref)
{
	FChunkStruct base;
	ChunkDimensions = NodeDimensions * base.ChunkSize;
	ActionGrammarsRef = ref;

	FTurningValues* turningValues = ActionGrammarsRef->GetTurnValues();
	turningValues->DetermineChances();
}

/* FUTURE TO DO
* Right now being lazy and not going to implement the path nodes
* Path nodes are implemented to prevent environmental objects blocking the traversal path
* Since we don't have environmental objects yet I've not implemented this
*/
FVector UGeometryRealisationComponent::AddPathToGrid(TArray<FPathSection> Path, FVector PathOrigin, FVector& FacingDirection)
{

	FVector currentPosition = PathOrigin;

	SetStartingNode(PathOrigin);

	//FVector lineDistance;

	FGenerationPlatform generatedPlatform;
	float rotationAngle;
	FVector platformRotation;

	FGenerationCollectables generatedCollectable;

	generatedPlatform.startPosition = currentPosition;
	currentPosition += FVector(
		FacingDirection.X,
		FacingDirection.Y,
		0.0f) * 160.0f;
	generatedPlatform.endPosition = currentPosition;
	generatedPlatform.rotation = FacingDirection.Rotation();
	GeneratedPlatforms.Add(generatedPlatform);

	//UE_LOG(LogTemp, Log,
		//TEXT("Geometry Realising"));

	for (FPathSection PathSection : Path) {
		if (PathSection.IsMove)
		{
			// ATTEMPT TURN DIRECTION
			generatedPlatform.startPosition = currentPosition;

			rotationAngle = FMath::DegreesToRadians(GenerateTurnAngle());

			FacingDirection = FacingDirection.GetSafeNormal().RotateAngleAxis(rotationAngle, FVector::UpVector);

			currentPosition += FVector(
				FacingDirection.X,
				FacingDirection.Y,
				0.0f) * PathSection.TravelVector.X
				+ FVector(0.0f, 0.0f, PathSection.TravelVector.Z);

			generatedPlatform.endPosition = currentPosition;
			platformRotation = FVector(FacingDirection.X, FacingDirection.Y, PathSection.TravelVector.GetSafeNormal().Z);
			platformRotation = platformRotation.GetSafeNormal();
			generatedPlatform.rotation = platformRotation.Rotation();
			GeneratedPlatforms.Add(generatedPlatform);

			// ADD PATH SECTION
			//DrawDebugLine(GetWorld(), generatedPlatform.startPosition, generatedPlatform.endPosition, FColor::Red, true, -1.0f, (uint8)0U, 10.0f);
		}
		else if (PathSection.IsJump){
			//UE_LOG(LogTemp, Log,
				//TEXT("Jump"));
			rotationAngle = GenerateTurnAngle();

			FacingDirection = FacingDirection.GetSafeNormal().RotateAngleAxis(rotationAngle, FVector::UpVector);

			currentPosition += FVector(
				FacingDirection.X,
				FacingDirection.Y,
				0.0f) * PathSection.TravelVector.X
				+ FVector(0.0f, 0.0f, PathSection.TravelVector.Z);

			//DrawDebugLine(GetWorld(), generatedPlatform.endPosition, currentPosition, FColor::Green, true, -1.0f, (uint8)0U, 10.0f);
			
		}

		int iterations;
		float travelPercentage;
		// Collectable generation
		if (FMath::FRandRange(0.0f, 1.0f) <= ActionGrammarsRef->GetCollectableValues().CoinGenerationChance) {
			switch (PathSection.collectiblePlacementType)
			{
			case ECollectiblePlacementType::None:
				break;
			case ECollectiblePlacementType::AboveHorizontalPositions:
				iterations = PathSection.TravelVector.X / ActionGrammarsRef->GetCollectableValues().CoinSeperationDistance;
				for (float i = 1; i <= iterations; i += 1.0f) {
					travelPercentage = (i * ActionGrammarsRef->GetCollectableValues().CoinSeperationDistance)
						/ PathSection.TravelVector.X;
					//UE_LOG(LogTemp, Log,
						//TEXT("Percentage %f"), travelPercentage);

					generatedCollectable.position = generatedPlatform.startPosition +
						FVector(
							FacingDirection.X,
							FacingDirection.Y,
							0.0f)
						* ActionGrammarsRef->GetCollectableValues().CoinSeperationDistance
						* i
						+ FVector(
							0.0f,
							0.0f,
							FMath::Lerp(
								generatedPlatform.startPosition.Z,
								generatedPlatform.endPosition.Z,
								travelPercentage)
							- generatedPlatform.startPosition.Z
						);
					GeneratedCollectables.Add(generatedCollectable);
				}
				break;
			case ECollectiblePlacementType::UpStream:

				break;
			case ECollectiblePlacementType::UpArc:

				break;
			case ECollectiblePlacementType::StraightArc:

				break;
			case ECollectiblePlacementType::DownArc:

				break;
			default:
				break;
			}
		}
		

		if (IncludeSafetyPlatforms) {
			generatedPlatform.startPosition = currentPosition;
			currentPosition += FVector(
				FacingDirection.X,
				FacingDirection.Y,
				0.0f) * 160.0f;
			generatedPlatform.endPosition = currentPosition;
			generatedPlatform.rotation = FacingDirection.Rotation();
			GeneratedPlatforms.Add(generatedPlatform);
		}
	}

	generatedPlatform.startPosition = currentPosition;
	currentPosition += FVector(
		FacingDirection.X,
		FacingDirection.Y,
		0.0f) * 160.0f;
	generatedPlatform.endPosition = currentPosition;
	generatedPlatform.rotation = FacingDirection.Rotation();
	GeneratedPlatforms.Add(generatedPlatform);

	return currentPosition;
}

// Return value is mainly for the Get function bellow
FChunkStruct& UGeometryRealisationComponent::AddChunkToGrid(FVector ChunkPosition)
{
	FBox chunkBox;

	for (FChunkStruct& ExistingChunk : GeometryGrid) {
		chunkBox = GetChunkBoxDimensions(ExistingChunk.ChunkOrigin);
		if (chunkBox.IsInside(ChunkPosition)) {
			//Chunk exists
			return ExistingChunk;
		}
	}

	FChunkStruct newChunk;
	newChunk.SetPosition(GetCenterOfChunkInPosition(ChunkPosition), NodeDimensions);
	int32 ChunkIndex = GeometryGrid.Add(newChunk);

	return GeometryGrid[ChunkIndex];
}

FChunkStruct& UGeometryRealisationComponent::GetChunkFromGrid(FVector ChunkPosition)
{
	FBox chunkBox;

	for (FChunkStruct& ExistingChunk : GeometryGrid) {
		chunkBox = GetChunkBoxDimensions(ExistingChunk.ChunkOrigin);
		if (chunkBox.IsInside(ChunkPosition)) {
			return ExistingChunk;
		}
	}

	return AddChunkToGrid(ChunkPosition);
}

FBox UGeometryRealisationComponent::GetChunkBoxDimensions(FVector ChunkPosition)
{
	FVector NormalisedChunkPosition = GetCenterOfChunkInPosition(ChunkPosition);

	FVector chunkBottomLeft = NormalisedChunkPosition - ChunkDimensions / 2.0f;
	FVector chunkTopRight = NormalisedChunkPosition + ChunkDimensions / 2.0f;

	FBox chunkBox(chunkBottomLeft, chunkTopRight);
	return chunkBox;
}

FVector UGeometryRealisationComponent::GetCenterOfChunkInPosition(FVector ChunkPosition)
{
	// Normalise position to be on the grid
	FVector Distance = ChunkPosition - GridOrigin;

	if (Distance.X * Distance.X < (ChunkDimensions.X / 2.0f) * (ChunkDimensions.X / 2.0f) &&
		Distance.Y * Distance.Y < (ChunkDimensions.Y / 2.0f) * (ChunkDimensions.Y / 2.0f) &&
		Distance.Z * Distance.Z < (ChunkDimensions.Z / 2.0f) * (ChunkDimensions.Z / 2.0f)) {
		return GridOrigin;
	}

	FVector DistanceSign = FVector(FMath::Sign(Distance.X), FMath::Sign(Distance.Y), FMath::Sign(Distance.Z));

	Distance -= DistanceSign * ChunkDimensions / 2.0f;

	int32 ChunkX = FMath::FloorToInt(Distance.X / ChunkDimensions.X) + DistanceSign.X;
	int32 ChunkY = FMath::FloorToInt(Distance.Y / ChunkDimensions.Y) + DistanceSign.Y;
	int32 ChunkZ = FMath::FloorToInt(Distance.Z / ChunkDimensions.Z) + DistanceSign.Z;

	FVector ChunkCenter(
		GridOrigin.X + ChunkX * ChunkDimensions.X,
		GridOrigin.Y + ChunkY * ChunkDimensions.Y,
		GridOrigin.Z + ChunkZ * ChunkDimensions.Z
	);

	return ChunkCenter;
}

void UGeometryRealisationComponent::SetStartingNode(FVector NodePosition)
{
	int iterations = 0;
	SetNodeAtPosition(NodePosition, ENodeType::Platform);
	for (float i = NodeDimensions.Z; i < ActionGrammarsRef->GetPlayersValues().Height * 100.0f; i += NodeDimensions.Z) {
		SetNodeAtPosition(NodePosition + FVector(0, i, 0), ENodeType::Path);
		iterations++;
	}
	SetNodeAtPosition(NodePosition + FVector(0, (iterations + 1) * NodeDimensions.Z, 0), ENodeType::Path);
}

void UGeometryRealisationComponent::SetNodeAtPosition(FVector NodePosition, ENodeType NodeType)
{
	FChunkStruct& CurrentChunk = GetChunkFromGrid(NodePosition);

	FNodeStruct& CurrentNode = CurrentChunk.GetNodeFromPosition(NodePosition);

	CurrentNode.NodeType = NodeType;
}

void UGeometryRealisationComponent::SetNodeBlockingAtPosition(FVector NodePosition, bool isBlocking)
{
	FChunkStruct& CurrentChunk = GetChunkFromGrid(NodePosition);

	FNodeStruct& CurrentNode = CurrentChunk.GetNodeFromPosition(NodePosition);
	
	CurrentNode.isBlocking = isBlocking;
}

float UGeometryRealisationComponent::GenerateTurnAngle()
{
	FTurningValues* turningValues = ActionGrammarsRef->GetTurnValues();

	float turnTotalChanceValues = turningValues->NoTurnChance
		+ turningValues->SmallTurnChance
		+ turningValues->LargeTurnChance;
	float turnRandomValue = FMath::FRandRange(0.0f, turnTotalChanceValues);

	float returnAngle = 0.0f;

	if (turnRandomValue <= turningValues->NoTurnChance) {
		return returnAngle;
	}
	else if (turnRandomValue <= turningValues->NoTurnChance + turningValues->SmallTurnChance) {
		returnAngle = turningValues->SmallTurnAngle;
	}
	else {
		returnAngle = turningValues->LargeTurnAngle;
	}

	if (FMath::FRandRange(0.0f, turningValues->LeftTurnChance + turningValues->RightTurnChance)
		<= turningValues->RightTurnChance) {
		returnAngle *= -1;
	}

	return returnAngle;
}

FNodeStruct UGeometryRealisationComponent::GetNodeAtPosition(FVector NodePosition)
{
	FChunkStruct CurrentChunk = GetChunkFromGrid(NodePosition);

	return CurrentChunk.GetNodeFromPosition(NodePosition);
}

// I'm tired so being lazy but this doesn't cover all the potential platform positions
// The issue arrises with diagonals
// Look in IsStraightPathFree for how to handle diagonals
TArray<FVector> UGeometryRealisationComponent::GetPlayerNodesAtPosition(FVector CenterBottom, FVector FacingDirection, bool onPlatform)
{
	TArray<FVector> returnValues;

	FVector BottomLeft = CenterBottom - ((ActionGrammarsRef->GetPlayersValues().Width / 2.0f) * 100.0f);
	FVector distanceVector;

	// Guarantee new vectors within
	for (float width = 0; width < ActionGrammarsRef->GetPlayersValues().Width; width += 1.0f) {
		for (float height = 0; height < ActionGrammarsRef->GetPlayersValues().Height; height += 1.0f) {
			distanceVector = FacingDirection.RightVector * width * 100.0f;
			distanceVector += BottomLeft + FVector(0.0f, height * 100.0f, 0.0f);
			returnValues.Add(GetNodeAtPosition(distanceVector).Position);
		}
	}

	for (float width = 0; width < ActionGrammarsRef->GetPlayersValues().Width; width += 1.0f) {
		distanceVector = FacingDirection.RightVector * width * 100.0f;
		distanceVector += BottomLeft + FVector(0.0f, ActionGrammarsRef->GetPlayersValues().Height * 100.0f, 0.0f);
		if (!returnValues.Contains(GetNodeAtPosition(distanceVector).Position)) {
			returnValues.Add(GetNodeAtPosition(distanceVector).Position);
		}
	}

	for (float height = 0; height < ActionGrammarsRef->GetPlayersValues().Height; height += 1.0f) {
		distanceVector = FacingDirection.RightVector * ActionGrammarsRef->GetPlayersValues().Width * 100.0f;
		distanceVector += BottomLeft + FVector(0.0f, height * 100.0f, 0.0f);
		if (!returnValues.Contains(GetNodeAtPosition(distanceVector).Position)) {
			returnValues.Add(GetNodeAtPosition(distanceVector).Position);
		}
	}

	if (onPlatform) {
		returnValues.Add(GetNodeAtPosition(BottomLeft - FVector(0.0f, 0.0f, NodeDimensions.Z)).Position);
		returnValues.Add(GetNodeAtPosition(BottomLeft - FVector(0.0f, 0.0f, NodeDimensions.Z) + (FacingDirection.RightVector * NodeDimensions.X)).Position);
	}

	return returnValues;
}

bool UGeometryRealisationComponent::IsNodeOpen(FVector NodePosition)
{
	FChunkStruct CurrentChunk = GetChunkFromGrid(NodePosition);

	FNodeStruct CurrentNode = CurrentChunk.GetNodeFromPosition(NodePosition);

	return CurrentNode.NodeType == ENodeType::Open;
}

bool UGeometryRealisationComponent::IsStraightPathFree(FVector StartPosition, FVector TravelVector)
{
	FVector currentNodePosition = GetNodeAtPosition(StartPosition).Position;
	FVector currentTraversalPosition = StartPosition;
	FVector centerOfNodeToPosition = currentTraversalPosition - currentNodePosition;
	float totalDistanceTraveled = 0.0f;
	FVector nodeTraversalRemaining;

	FVector nodeTraversalPercentagesRemaining;
	FVector traversalMovementTraveled;

	float distanceToTravel = sqrt(TravelVector.X * TravelVector.X
		+ TravelVector.Y * TravelVector.Y
		+ TravelVector.Z * TravelVector.Z);
	FVector MovementDirection = TravelVector.GetSafeNormal();
	if (MovementDirection == FVector::ZeroVector) {
		return false;
	}

	while (totalDistanceTraveled < distanceToTravel) {
		if (!IsNodeOpen(currentNodePosition)) {
			return false;
		}

		nodeTraversalRemaining = FVector(
			(NodeDimensions.X / 2.0f) - (centerOfNodeToPosition.X * (MovementDirection.X >= 0 ? 1 : -1)),
			(NodeDimensions.Y / 2.0f) - (centerOfNodeToPosition.Y * (MovementDirection.Y >= 0 ? 1 : -1)),
			(NodeDimensions.Z / 2.0f) - (centerOfNodeToPosition.Z * (MovementDirection.Z >= 0 ? 1 : -1))
		);

		if (MovementDirection.X != 0) {
			nodeTraversalPercentagesRemaining.X = 
				(nodeTraversalRemaining.X * nodeTraversalRemaining.X) / (MovementDirection.X * MovementDirection.X);
		}
		else {
			nodeTraversalPercentagesRemaining.X = FLT_MAX;
		}

		if (MovementDirection.Y != 0) {
			nodeTraversalPercentagesRemaining.Y =
				(nodeTraversalRemaining.Y * nodeTraversalRemaining.Y) / (MovementDirection.Y * MovementDirection.Y);
		}
		else {
			nodeTraversalPercentagesRemaining.Y = FLT_MAX;
		}

		if (MovementDirection.Z != 0) {
			nodeTraversalPercentagesRemaining.Z =
				(nodeTraversalRemaining.Z * nodeTraversalRemaining.Z) / (MovementDirection.Z * MovementDirection.Z);
		}
		else {
			nodeTraversalPercentagesRemaining.Z = FLT_MAX;
		}

		if (nodeTraversalPercentagesRemaining.X <= nodeTraversalPercentagesRemaining.Y
			&& nodeTraversalPercentagesRemaining.X <= nodeTraversalPercentagesRemaining.Z) {

			currentNodePosition = GetNodeAtPosition(
				currentNodePosition + (NodeDimensions.X * (MovementDirection.X >= 0 ? 1 : -1))).Position;

			traversalMovementTraveled = MovementDirection * nodeTraversalPercentagesRemaining.X;
		}
		else if (nodeTraversalPercentagesRemaining.Y <= nodeTraversalPercentagesRemaining.Z) {
			currentNodePosition = GetNodeAtPosition(
				currentNodePosition + (NodeDimensions.Y * (MovementDirection.Y >= 0 ? 1 : -1))).Position;

			traversalMovementTraveled = MovementDirection * nodeTraversalPercentagesRemaining.Y;
		}
		else {
			currentNodePosition = GetNodeAtPosition(
				currentNodePosition + (NodeDimensions.Z * (MovementDirection.Z >= 0 ? 1 : -1))).Position;

			traversalMovementTraveled = MovementDirection * nodeTraversalPercentagesRemaining.Z;
		}

		currentTraversalPosition += traversalMovementTraveled;
		// Magnitude calculation
		totalDistanceTraveled += sqrt(traversalMovementTraveled.X * traversalMovementTraveled.X
			+ traversalMovementTraveled.Y * traversalMovementTraveled.Y
			+ traversalMovementTraveled.Z * traversalMovementTraveled.Z);
	}

	return true;
}

float UGeometryRealisationComponent::GenerateLevel()
{
	if (!StandardPlatform) {
		//UE_LOG(LogTemp, Warning, TEXT("No Base Platform"));
		return 0.0f;
	}
	if (!StandardCollectable) {
		//UE_LOG(LogTemp, Warning, TEXT("No Collectable"));
		return 0.0f;
	}

	float defaultPlatformUnitWidth = ActionGrammarsRef->GetPlatformValues().RegularPlatformDefaultSize.Y;

	float lowestPosition = 0.0f;

	for (int i = 0; i < GeneratedPlatforms.Num(); i++) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = GetOwner();

		FVector platformStartPosition = GeneratedPlatforms[i].startPosition;
		FVector platformEndPosition = GeneratedPlatforms[i].endPosition;
		FRotator platformRotation = GeneratedPlatforms[i].rotation;

		lowestPosition = fmin(lowestPosition, fmin(platformStartPosition.Z, platformEndPosition.Z));

		AActor* spawnedPlatform = GetWorld()->SpawnActor<AActor>(
			StandardPlatform,
			(platformEndPosition + platformStartPosition) / 2.0f,
			platformRotation,
			spawnParams);

		spawnedPlatform->AttachToActor(
			GetOwner(),
			FAttachmentTransformRules::KeepRelativeTransform
		);

		FVector distance = platformEndPosition - platformStartPosition;
		float distanceMagnitude = sqrt(
			(distance.X * distance.X)
			+ (distance.Y * distance.Y)
			+ (distance.Z * distance.Z)
		);

		spawnedPlatform->SetActorScale3D(FVector(
			1.0f,
			distanceMagnitude / defaultPlatformUnitWidth,
			1.0f));

		GeneratedPlatforms[i].platformRef = spawnedPlatform;
	}

	for (int i = 0; i < GeneratedCollectables.Num(); i++) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = GetOwner();

		FVector collectableSpawnPosition = GeneratedCollectables[i].position;

		AActor* spawnedCollectable = GetWorld()->SpawnActor<AActor>(
			StandardCollectable,
			collectableSpawnPosition,
			FRotator::ZeroRotator,
			spawnParams);

		spawnedCollectable->AttachToActor(
			GetOwner(),
			FAttachmentTransformRules::KeepRelativeTransform
		);

		GeneratedCollectables[i].collectableRef = spawnedCollectable;
	}

	return lowestPosition;
}

void UGeometryRealisationComponent::RemoveGeneratedLevel()
{
	for (int i = 0; i < GeneratedPlatforms.Num(); i++)
	{
		AActor* Platform = GeneratedPlatforms[i].platformRef;

		if (IsValid(Platform))
		{
			Platform->Destroy();
		}

		GeneratedPlatforms[i].platformRef = nullptr;
	}

	for (int i = 0; i < GeneratedCollectables.Num(); i++)
	{
		AActor* Collectable = GeneratedCollectables[i].collectableRef;

		if (IsValid(Collectable))
		{
			Collectable->Destroy();
		}

		GeneratedCollectables[i].collectableRef = nullptr;
	}

	GeneratedPlatforms.Empty();
	GeneratedCollectables.Empty();
}

void UGeometryRealisationComponent::ClearGrid()
{
	GeometryGrid.Empty();
}

void UGeometryRealisationComponent::SpawnPlatformAtPosition(FVector Position)
{
	/*FActorSpawnParameters spawnParams;
	spawnParams.Owner = GetOwner();
	AActor* spawnedPlatform = GetWorld()->SpawnActor<AActor>(
		StandardPlatform,
		Position,
		FRotator::ZeroRotator,
		spawnParams);

	spawnedPlatform->AttachToActor(
		GetOwner(),
		FAttachmentTransformRules::KeepRelativeTransform
	);

	GeneratedPlatforms.Add(spawnedPlatform);*/
}

void UGeometryRealisationComponent::EvaluateGeometry(UEvaluationPCGComponent* evaluationComponent)
{

	FVector startPosition = GeneratedPlatforms[0].startPosition;
	FVector endPosition = GeneratedPlatforms[GeneratedPlatforms.Num() - 1].endPosition;

	FVector levelLinearLine = endPosition - startPosition;

	for(FGenerationPlatform& platform : GeneratedPlatforms)
	{
		// Linearity
		FVector platformMiddle = (platform.startPosition + platform.endPosition) / 2.0f;

		FVector toPlatformVector = platformMiddle - startPosition;

		FVector closestParallelLine = FVector::CrossProduct(toPlatformVector, levelLinearLine);

		float distanceFromLinearLine = closestParallelLine.Size() / levelLinearLine.Size();


		evaluationComponent->AddPlatformDistance(distanceFromLinearLine);

		// Platform Density
		float platformVolume =
			(platform.endPosition - platform.startPosition).Size()
			* PlatformDimensions.X * PlatformDimensions.Z;



		evaluationComponent->AddObjectVolume(platformVolume);

	}


	float ChunkVolume = 
		NodeDimensions.X * GeometryGrid[0].ChunkSize
		* NodeDimensions.Y * GeometryGrid[0].ChunkSize
		* NodeDimensions.Z * GeometryGrid[0].ChunkSize;


	evaluationComponent->SetLevelVolume(GeometryGrid.Num() * ChunkVolume);

}



