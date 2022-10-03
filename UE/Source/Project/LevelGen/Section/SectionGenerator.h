#pragma once

#include "CoreMinimal.h"
#include "SectionGenerator.generated.h"

class USectionCompBase;
struct FProcSection;
struct FProcRoom;
class USectionDataConfig;
class ALevelEndLocation;

UCLASS()
class PROJECT_API ASectionGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASectionGenerator();
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor, Category="Generation")
	void Generate();

	UFUNCTION(CallInEditor, Category="Generation")
	void Clear();
	
	ALevelEndLocation* GetLevelEnd() const { return myLevelEnd.Get(); }
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
	int myNumSections = 3;
	
	// Different configs depending on difficulty? 
	UPROPERTY(EditAnywhere, Category="Generation")
	TArray<TSubclassOf<USectionDataConfig>> myConfigs;

	UPROPERTY(EditAnywhere)
	TObjectPtr<ALevelEndLocation> myLevelEnd;
	
private:

	USectionDataConfig* GetRandomConfig() const;
	
	void GenerateSection(FProcSection& aSection, const USectionDataConfig& aConfig, const FVector2D& aStartLocation) const;
	void GenerateLastEdge(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateWalls(FProcSection& aSection, const USectionDataConfig& aConfig) const;
	void GenerateGroundCeil(FProcSection& aSection, const USectionDataConfig& aConfig, const float aStartHeight) const;
	void Populate(FProcSection& aSection, const USectionDataConfig& aConfig);
	
	static TArray<USectionCompBase*> FilterSortRoomComponents(const FProcRoom& aRoom);
	static void AddSortedComponent(const TArray<USectionCompBase*>& aBase, TArray<USectionCompBase*>& aResult, USectionCompBase* aComp, const int32 aDepth);
	
	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
