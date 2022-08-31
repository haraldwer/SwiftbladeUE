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
	int32 startIndex;
	UPROPERTY(BlueprintReadOnly)
	int32 endIndex;
	UPROPERTY(BlueprintReadOnly)
	FVector2D normal;
	UPROPERTY(BlueprintReadOnly)
	FVector2D location;
	UPROPERTY(BlueprintReadOnly)
	bool isPath = false;
};

USTRUCT(BlueprintType)
struct FLevelDataWall
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> verts;
};

USTRUCT(BlueprintType)
struct FLevelDataRoom
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector2D location = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FVector2D center = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> vertices;
	UPROPERTY(BlueprintReadOnly)
	TArray<FLevelDataEdge> edges;
	UPROPERTY(BlueprintReadOnly)
	TArray<FLevelDataWall> walls;
	UPROPERTY(BlueprintReadOnly)
	float radius = 0.0f;
};