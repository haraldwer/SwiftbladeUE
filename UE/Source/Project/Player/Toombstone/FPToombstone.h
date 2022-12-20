#pragma once

#include "CoreMinimal.h"
#include "Project/Player/FPComponentBase.h"
#include "FPToombstone.generated.h"

UCLASS(Blueprintable)
class PROJECT_API AToombstone : public AActor
{
	GENERATED_BODY()
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPToombstone : public UFPActorComponentBase
{
	GENERATED_BODY()

public:

	UFPToombstone();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreateStones() const;
	void StoreLocation() const; 

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AToombstone> myToombstoneClass;
	
	UPROPERTY(EditDefaultsOnly)
	float myTraceDist = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 myNumToombstones = 10;
	
private:

	FVector myLastValidLocation = FVector::ZeroVector;
	
};
