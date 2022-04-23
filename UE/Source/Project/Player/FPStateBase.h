#pragma once

#include "CoreMinimal.h"
#include "Project/Utility/Tools/StateMachine/StateBase.h"
#include "FPStateBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPStateBase : public UStateBase
{
	GENERATED_BODY()

public:
	// Getters
	class AFPController& GetController() const;
	class AFPCharacter& GetCharacter() const;
	FTransform GetActorTransform() const;
	class UCapsuleComponent& GetCapsule() const;
	class UCharacterMovementComponent& GetCharacterMovement() const;
	class UCustomCamera& GetCamera() const;
	class UFPCamera& GetFPCamera() const;
	class UFPAnimatorNew& GetAnimator() const;
	class UFPMovement& GetMovement() const;
	class UFPCombat& GetCombat() const;
	float GetTime() const { return GetWorld()->GetTimeSeconds(); }
};
