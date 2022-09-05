#pragma once

#include "CoreMinimal.h"
#include "SectionGenerator.generated.h"

struct FProcSection;
class USectionDataConfig;

UCLASS()
class PROJECT_API ASectionGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASectionGenerator();
	virtual void BeginPlay() override;

	bool GetDrawDebug() const { return myDebugDrawNoise; }
	
	UFUNCTION(BlueprintCallable)
	void AddGeneratedObject(UObject* anObject);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans);

protected:

	UPROPERTY(EditAnywhere, Category="Generation")
	bool myDebugDrawNoise = false;

	UPROPERTY(EditAnywhere, Category="Generation")
	int32 mySeed = 0;
	
	UPROPERTY(EditAnywhere, Category="Generation")
	TSubclassOf<USectionDataConfig> myConfig;
	
	UFUNCTION(CallInEditor, Category="Generation")
	void Generate();

	UFUNCTION(CallInEditor, Category="Generation")
	void Clear();
	
private:

	USectionDataConfig* GetConfig() const;
	
	void GenerateSection(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateLastEdge(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateWalls(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateGroundCeil(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void Populate(const FProcSection& aSection, const USectionDataConfig& aConfig);
	
	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
