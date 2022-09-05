#pragma once

#include "CoreMinimal.h"
#include "SectionDataStructs.generated.h"

USTRUCT(BlueprintType)
struct FProcEdge
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
struct FProcWall
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> verts;
};

USTRUCT(BlueprintType)
struct FProcRoom
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector2D location = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FVector2D center = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> vertices;
	UPROPERTY(BlueprintReadOnly)
	TArray<FProcEdge> edges;
	UPROPERTY(BlueprintReadOnly)
	TArray<FProcWall> walls;
	UPROPERTY(BlueprintReadOnly)
	float radius = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float groundOffset = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float ceilHeight = 0.0f;
};

USTRUCT(BlueprintType)
struct FProcSection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FProcRoom> rooms;
	UPROPERTY(BlueprintReadOnly)
	FVector2D lastEdgeLoc;
};