#pragma once

#include "CoreMinimal.h"
#include "Project/Player/FPStateBase.h"
#include "Project/Player/Movement/FPMovementInputAction.h"
#include "FPMovementStateBase.generated.h"

class UFPAnimationStateBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateBase : public UFPStateBase
{
	GENERATED_BODY()

public:
	virtual ~UFPMovementStateBase() override = default;

	// Override for custom behaviour
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue);
	virtual UClass* OnCrouch(bool aStartedCrouch, float aHalfHeightAdjust) { return nullptr; }
	virtual UClass* OnLanded() { return nullptr; }
	virtual UClass* OnHit(const FHitResult& aHit) { return nullptr; }

	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const { return {}; }
	virtual bool RequiresMagic() const { return false; }
	virtual bool CanJump() const { return true; }

	virtual void ResetAbilities();

protected:

	// Helpers
	void Jump(FVector aDirection = FVector(0, 0, 1)) const;
	void MoveHorizontal(float aValue) const;
	void MoveVertical(float aValue) const;
	void LookHorizontal(float aValue) const;
	void LookVertical(float aValue) const;

	bool HasMagic() const;

	TArray<FHitResult> MultiSweep(const FVector& aStart, const FVector& aEnd, const float aRadius = 0.0f) const;
	FHitResult Sweep(const FVector& aStart, const FVector& aEnd, const float aRadius = 0.0f) const;
	
	// Override for custom modification of input values
	virtual float GetSensitivity() const;
};
