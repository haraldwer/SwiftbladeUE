#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project/Enemies/Enemy.h"
#include "Sword.generated.h"

UCLASS()
class PROJECT_API ASword : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASword();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetPlayer(class AFPCharacter* aPlayer);
	void Return();

	FTransform GetHitTransform(const AActor* anActor) const;
	void CreateHitEffect(const FTransform& aTrans) const;

	UFUNCTION(BlueprintImplementableEvent)
	void SetCrystalsActive(int32 aCrystalCount, bool aShowEffect);

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Before pickup")
	float myRotationSpeed = 15.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Before pickup")
	float myPositionSpeed = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Before pickup")
	float myPositionScale = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Hit Effect")
	float myEffectStartOffset = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Hit Effect")
	float myEffectEndOffset = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEffect> myHitEffectBP;

private:
	
	TWeakObjectPtr<AFPCharacter> myPlayer;
	
	float myTimer;
	FVector myStartLocation;

};
