#pragma once

#include "CoreMinimal.h"
#include "ArenaDataStructs.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "ArenaGenerator.generated.h"

struct FArenaConfigLayer;
class ACheckpoint;
class UArenaConfig;

UCLASS()
class PROJECT_API AArenaGenerator : public AGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void Generate() override;

protected:

	UPROPERTY(EditAnywhere, Category="Generation")
	TArray<TSubclassOf<UArenaConfig>> myConfigs;

private:

	void CreateCheckpoint(const UArenaConfig* aConfig, const FArenaLayer& aLayer);
	void CreateDoor(const UArenaConfig* aConfig, const FArenaLayer& aLayer);
	
	const UArenaConfig* GetRandomConfig() const;
	
};
