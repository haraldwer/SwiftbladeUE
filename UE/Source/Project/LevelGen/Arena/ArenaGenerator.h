#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "ArenaGenerator.generated.h"

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

	UArenaConfig* GetRandomConfig() const;
	
};
