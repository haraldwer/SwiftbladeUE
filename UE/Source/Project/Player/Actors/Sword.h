#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Sword.generated.h"

UCLASS()
class PROJECT_API ASword : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASword();

	TArray<AActor*> GetOverlaps(UClass* aClass);
	void SetPlayer(class AFPCharacter* aPlayer);
	void Return();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class AFPCharacter* myPlayer;
	
	UPROPERTY(EditAnywhere, Category = "Before pickup")
	float myRotationSpeed = 15.0f;
	UPROPERTY(EditAnywhere, Category = "Before pickup")
	float myPositionSpeed = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Before pickup")
	float myPositionScale = 50.0f;
	
	float myTimer;
	FVector myStartLocation;
};
