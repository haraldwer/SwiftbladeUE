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
	AActor* myHitActor = nullptr;
	FTransform myTransform;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateBase : public UFPStateBase
{
	GENERATED_BODY()

public:
	
	virtual UClass* Update(float aDT) override;
	
protected:

	float GetStateTime() const { return myTime; }
	void ResetStateTime() { myTime = 0; }
	
	// Set / Get hand positions
	
	void SetHands(const FFPAnimationHandPositions& someHandPositions, bool aSnap = false) const;
	FFPAnimationHandPositions GetTargetHands() const;
	FFPAnimationHandPositions GetRealHands() const;

	void SetCamera(const FFPAnimationCameraData& someCameraData, bool aSnap = false) const;
	FFPAnimationCameraData GetTargetCamera() const;
	FFPAnimationCameraData GetRealCamera() const;
	
	// Helper functions

	static FTransform FlipRightToLeft(const FTransform& aRightTransform);
	FTransform ToCameraSpace(const FTransform& aTransform) const;
	FVector Get3DNoise(float aSpeed, float aStrength, float anOffset = 0.0f) const;
	FFPAnimationHandCollision GetHandCollision(const FTransform& aHandTrans, float aRadius) const;

	void OverrideSwordData(FFPAnimationHandPositions& someData, float aLocationWeight, float aRotationWeight, bool aDualWeild) const;
	void OverrideVelocityData(FFPAnimationHandPositions& someData, float aVelocityWeight, float aDT) const;
	
	float GetVelocityCameraTilt() const;

	// Properties
	
	FTransform GetDefaultHandTransform() const;

private:
	float myTime = 0.0f;
};
