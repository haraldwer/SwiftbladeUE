#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeneratorBase.generated.h"

UCLASS()
class PROJECT_API AGeneratorBase : public AActor
{
	GENERATED_BODY()

public:
	
	AGeneratorBase();

	UFUNCTION(CallInEditor, Category="Generation")
	void Generate() { Generate(nullptr); }
	
	virtual void Generate(class ULevelSubsystem* aLevelSubsystem);

	UFUNCTION(CallInEditor, Category="Generation")
	virtual void Clear();

	UFUNCTION(BlueprintCallable)
	void AddGeneratedObject(UObject* anObject);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans);

protected:
	
	UPROPERTY(EditAnywhere, Category="Generation")
	int32 mySeed = 0;

private:

	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
