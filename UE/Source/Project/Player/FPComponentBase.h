#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPComponentBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFPComponentBase();

	class AFPController& GetController() const;
	class AFPCharacter& GetCharacter() const;
	FTransform GetActorTransform() const;
	class UCapsuleComponent& GetCapsule() const;
	class UCharacterMovementComponent& GetCharacterMovement() const;
	class UCameraComponent& GetCamera() const;
	class UFPAnimator& GetAnimator() const;
	class UFPMovement& GetMovement() const;
	class UFPCombat& GetCombat() const;
};
