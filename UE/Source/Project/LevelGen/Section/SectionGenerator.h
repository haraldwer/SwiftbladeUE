#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "SectionGenerator.generated.h"

class USectionCompBase;
struct FProcSection;
struct FProcRoom;
class USectionDataConfig;
class ALevelEnd;

UCLASS()
class PROJECT_API ASectionGenerator : public AGeneratorBase
{
	GENERATED_BODY()
	
public:

	virtual void Generate() override;
	
	ALevelEnd* GetLevelEnd() const { return myLevelEnd.Get(); }

protected:

	UPROPERTY(EditAnywhere, Category="Generation")
	bool myDebugDrawNoise = false;

	UPROPERTY(EditAnywhere, Category="Generation")
	int myNumSections = 5;
	
	// Different configs depending on difficulty? 
	UPROPERTY(EditAnywhere, Category="Generation")
	TArray<TSubclassOf<USectionDataConfig>> myConfigs;

	UPROPERTY(EditAnywhere, Category="Generation")
	TSubclassOf<ALevelEnd> myLevelEndClass;
	
private:

	USectionDataConfig* GetRandomConfig() const;
	
	void GenerateSection(FProcSection& aSection, const USectionDataConfig& aConfig, const FVector2D& aStartLocation) const;
	void GenerateLastEdge(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateWalls(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateRoomPath(FProcSection& aSection) const;
	void GenerateGroundCeil(FProcSection& aSection, const USectionDataConfig& aConfig, const float aStartHeight) const;
	void Populate(FProcSection& aSection, const USectionDataConfig& aConfig);

	TWeakObjectPtr<ALevelEnd> myLevelEnd; 
	
};
