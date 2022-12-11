#pragma once

#include "CoreMinimal.h"
#include "Utility/StateMachine/StateBase.h"
#include "FPStateBase.generated.h"

UCLASS(ClassGroup=(Custom))
class PROJECT_API  UFPStateBase : public UStateBase
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
	class UFPTime& GetTime() const; 

	class AEffect* CreateEffect(const TSubclassOf<AEffect>& aBP, const FTransform& aTransform) const;
	
	float GetCurrentTime() const { return GetWorld()->GetTimeSeconds(); }

	// If player should take damage in this state
	virtual bool TakeDamage(float ADamageAmount, FDamageEvent const& DamageEvent, AController* Controller, AActor* Actor)
	{ return true; }
};
