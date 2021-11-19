#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelData.generated.h"

UCLASS()
class PROJECT_API ALevelData : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelData();

	virtual void BeginPlay() override;
};
