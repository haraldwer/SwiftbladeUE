#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeneratedRock.generated.h"

UCLASS()
class PROJECT_API AGeneratedRock : public AActor
{
	GENERATED_BODY()

public:
	AGeneratedRock();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor, Category="Generation")
	void GenerateMesh() const;

	UPROPERTY(EditDefaultsOnly, Category="Generation")
	int mySubdivisions = 3;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UProceduralMeshComponent* myCustomMesh;
	
private:
	
};
