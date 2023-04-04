#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SectionComponent.generated.h"

class UChapterPreset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API USectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	void GenerateLevelOrder(int32 aChapter, TArray<FString>& someLevels, TArray<int32>& someArenaIndices) const;

protected:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UChapterPreset>> myChapters; 
	
};
