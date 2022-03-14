#pragma once

#include "CoreMinimal.h"
#include "Project/Player/Movement/FPMovementInputAction.h"
#include "Project/Utility/Tools/StateMachine/StateBase.h"
#include "FPMovementStateBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateBase : public UStateBase
{
	GENERATED_BODY()

public:
	virtual ~UFPMovementStateBase() override = default;

	// Override for custom behaviour
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue);
	virtual UClass* OnLanded() { return nullptr; }
	virtual UClass* OnHit(const FHitResult& aHit) { return nullptr; }

protected:

	// Helpers
	void Jump(FVector aDirection = FVector(0, 0, 1)) const;
	void MoveHorizontal(float aValue) const;
	void MoveVertical(float aValue) const;
	void LookHorizontal(float aValue) const;
	void LookVertical(float aValue) const;
	
	// Override for custom modification of input values
	virtual float GetSensitivity() const;

	// Getters
	class AFPController& GetController() const;
	class AFPCharacter& GetCharacter() const;
	FTransform GetActorTransform() const;
	class UCapsuleComponent& GetCapsule() const;
	class UCharacterMovementComponent& GetCharacterMovement() const;
	class UCameraComponent& GetCamera() const;
	class UFPCamera& GetFPCamera() const;
	class UFPAnimator& GetAnimator() const;
	class UFPMovementStateMachine& GetMovement() const;
	class UFPCombat& GetCombat() const;
	float GetTime() const { return GetWorld()->GetTimeSeconds(); }
};
