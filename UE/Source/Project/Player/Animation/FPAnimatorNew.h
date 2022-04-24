#pragma once

#include "CoreMinimal.h"
#include "FPAnimationCameraData.h"
#include "FPAnimationHandPositions.h"
#include "Project/Utility/Tools/StateMachine/StateMachine.h"
#include "FPAnimatorNew.generated.h"

UCLASS(ClassGroup=(Custom))
class PROJECT_API UFPAnimatorNew : public UStateMachine
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual UClass* GetDefaultStateType() override;

	// Set / Get for animation data
	
	void SetHands(const FFPAnimationHandPositions& someHandPositions, bool aSnap);
	FFPAnimationHandPositions GetTargetHands() const { return myTargetHands; }
	FFPAnimationHandPositions GetRealHands() const { return myRealHands; }
	
	void SetCamera(const FFPAnimationCameraData& someCameraData, bool aSnap);
	FFPAnimationCameraData GetTargetCamera() const { return myTargetCamera; }
	FFPAnimationCameraData GetRealCamera() const { return myRealCamera; }

	FTransform GetDefaultSwordTransform() const { return myDefaultSwordTransform; };
	
protected:

	UPROPERTY(EditDefaultsOnly)
	FTransform myDefaultSwordTransform;
	
private:
	class AFPCharacter& GetCharacter() const;
	
	void UpdateHands(float aDT);
	void UpdateCamera(float aDT);
	
	// Hands
	FFPAnimationHandPositions myTargetHands;
	FFPAnimationHandPositions myRealHands;

	FFPAnimationCameraData myTargetCamera;
	FFPAnimationCameraData myRealCamera;
};