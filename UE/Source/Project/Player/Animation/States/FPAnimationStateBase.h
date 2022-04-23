#pragma once

#include "CoreMinimal.h"
#include "Project/Player/FPStateBase.h"
#include "Project/Player/Animation/FPAnimationCameraData.h"
#include "Project/Player/Animation/FPAnimationHandPositions.h"
#include "FPAnimationStateBase.generated.h"

USTRUCT()
struct FFPAnimationHandCollision
{
	GENERATED_BODY()

	bool myHit = false;
	UPROPERTY()
	AActor* myHitActor;
	FTransform myTransform;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateBase : public UFPStateBase
{
	GENERATED_BODY()

public:
	
	virtual void Enter() override;
	virtual UClass* Update(float aDT) override;
	
protected:

	float GetStateTime() const { return myTime; }
	
	// Set / Get hand positions
	
	void SetHands(const FFPAnimationHandPositions& someHandPositions, bool aSnap = false) const;
	FFPAnimationHandPositions GetTargetHands() const;
	FFPAnimationHandPositions GetRealHands() const;

	void SetCamera(const FFPAnimationCameraData& someCameraData, bool aSnap = false) const;
	FFPAnimationCameraData GetTargetCamera() const;
	FFPAnimationCameraData GetRealCamera() const;
	
	// Helper functions

	static FTransform DualWeild(const FTransform& aRightTransform);
	static FTransform FlipRightToLeft(const FTransform& aRightTransform);
	FTransform ToCameraSpace(const FTransform& aTransform) const;
	EHandState GetSwordHandState() const;
	FVector Get3DNoise(float aSpeed, float aStrength, float anOffset = 0.0f) const;
	FFPAnimationHandCollision GetHandCollision(const FTransform& aHandTrans, float aRadius) const;
	
	// Properties
	
	FTransform GetDefaultHandTransform() const;

private:
	float myTime = 0.0f;
};
