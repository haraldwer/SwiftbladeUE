
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StickySurface.generated.h"

UCLASS()
class PROJECT_API AStickySurface : public AActor
{
	GENERATED_BODY()

public:
	AStickySurface();

	virtual void BeginPlay() override;
};
