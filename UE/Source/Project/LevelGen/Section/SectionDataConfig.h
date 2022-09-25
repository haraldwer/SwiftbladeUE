#pragma once

#include "CoreMinimal.h"
#include "SectionDataConfig.generated.h"

class USectionComponentBase;
class ASectionGenerator;

UCLASS(Blueprintable)
class USectionDataConfig : public UObject
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly)
	int32 myNumRooms = 4;
	
	UPROPERTY(EditDefaultsOnly, Category="Vertices")
	int32 myMinNumVerts = 4;
	UPROPERTY(EditDefaultsOnly, Category="Vertices")
	int32 myMaxNumVerts = 7;
	
	UPROPERTY(EditDefaultsOnly, Category="Room size")
	float myMinRoomRadius = 800.0f;
	UPROPERTY(EditDefaultsOnly, Category="Room size")
	float myMaxRoomRadius = 1200.0f;

	// Smallest possible distance between vertices
	UPROPERTY(EditDefaultsOnly)
	float myMinVertDist = 150.0f;
	// If should generate snake or choose straightest path
	UPROPERTY(EditDefaultsOnly, Category="Path")
	bool mySnakePath = true;
	// Snake Y threshold, increase for straighter snake
	UPROPERTY(EditDefaultsOnly, Category="Path")
	float mySnakePathY = 0.2f; 

	UPROPERTY(EditDefaultsOnly, Category="Ground")
	float myGroundOffsetChance = 50.0f;
	UPROPERTY(EditDefaultsOnly, Category="Ground")
	float myGroundMinOffset = -500.0f;
	UPROPERTY(EditDefaultsOnly, Category="Ground")
	float myGroundMaxOffset = 500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Ceiling")
	float myCeilingMinHeight = 500.0f;
	UPROPERTY(EditDefaultsOnly, Category="Ceiling")
	float myCeilingMaxHeight = 500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TArray<TSubclassOf<USectionComponentBase>> myComponents;
	
};
