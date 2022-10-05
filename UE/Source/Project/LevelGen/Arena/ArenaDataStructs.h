#pragma once

#include "CoreMinimal.h"
#include "ArenaDataStructs.generated.h"

USTRUCT(BlueprintType)
struct FArenaSubdivision
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float height = 0.0f;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> vertices;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<class UArenaCompBase*> components;
	
};

USTRUCT(BlueprintType)
struct FArenaSection
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FArenaSubdivision> subdivisions;
	
};

USTRUCT(BlueprintType)
struct FArenaLayer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float startRadius = 0.0f;
	
	UPROPERTY(BlueprintReadOnly)
	float endRadius = 0.0f;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FArenaSection> sections;
	
};
