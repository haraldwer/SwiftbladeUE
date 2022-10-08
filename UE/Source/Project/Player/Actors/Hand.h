#pragma once

#include "CoreMinimal.h"
#include "HandStateEnum.h"
#include "GameFramework/Actor.h"
#include "Hand.generated.h"

UCLASS()
class PROJECT_API AHand : public AActor
{
	GENERATED_BODY()
	
public:
	
	AHand();
	virtual void BeginPlay() override;
	virtual void Tick(float aDT) override;
	
	void SetHandState(EHandState aState) const;
	
	void SetMagicStrength(float aValue);

protected:

	UPROPERTY(EditDefaultsOnly)
	float myMagicDecrease = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* myMaterial;
	
private:

	float myMagicStrength = 0.0f;

	UPROPERTY()
	UMaterialInstanceDynamic* myMatInst;

};
