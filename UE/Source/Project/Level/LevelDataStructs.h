#pragma once

#include "CoreMinimal.h"
#include "LevelDataStructs.generated.h"

UENUM()
enum class ELevelType : uint8
{
	NONE,
	DEFAULT,
	COUNT
};

USTRUCT(BlueprintType)
struct FLevelDataEdge
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector2D normal;
	UPROPERTY(BlueprintReadOnly)
	FVector2D location;
	UPROPERTY(BlueprintReadOnly)
	bool isPath = false;
	UPROPERTY(BlueprintReadOnly)
	bool hasWall = false;
};

USTRUCT(BlueprintType)
struct FLevelDataFace
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector2D location = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> vertices;
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, FLevelDataEdge> edges;
};