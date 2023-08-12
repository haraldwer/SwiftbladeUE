#pragma once

#include "CoreMinimal.h"
#include "GeneratorBase.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Room.generated.h"

USTRUCT()
struct FRoomPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<AActor>> myActors;
 
	UPROPERTY(EditAnywhere)
	float myDifficulty = 1.0f;
	
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EFPCharacterAbilities))
	int32 myRequiredAbilities;
};

UCLASS(Abstract)
class PROJECT_API ARoom : public AGeneratorBase
{
	GENERATED_BODY()

public:

	ARoom();

	virtual void BeginPlay() override;
	virtual void Generate(ULevelSubsystem* aLevelManager) override;
	
	FTransform GetEntry() const;
	FTransform GetExit() const;
	
	USplineComponent* GetPath() const { return myPath.Get(); }

protected:

	void GenerateRoom();
	void GenerateConnection(ULevelSubsystem* aLevelManager);

	UFUNCTION(CallInEditor)
	void UpdatePath() const;
	
	UPROPERTY(EditAnywhere, Category="Connections")
	float myConnectionWidth = 300.0f;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> myRoot;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> myEntry;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> myEntryMesh;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> myExit;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> myExitMesh;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USplineComponent> myPath;

	// Predefined obstacles and abilities

	UPROPERTY(EditAnywhere, Category="Presets")
	TArray<FRoomPreset> myPresets;

	UPROPERTY(EditAnywhere, Category="Presets")
	int32 myPresetIndex = -1; 

	UPROPERTY(EditAnywhere, Category="Connections")
	TArray<TSubclassOf<AActor>> myConnections; 
	
};
