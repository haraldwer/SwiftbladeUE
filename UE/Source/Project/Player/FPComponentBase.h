#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "FPComponentBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPActorComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UFPActorComponentBase();
	
	// Getters for common stuff
	class AFPController& GetController() const;
	class AFPCharacter& GetCharacter() const;
	FTransform GetActorTransform() const;
	class UCapsuleComponent& GetCapsule() const;
	class UCharacterMovementComponent& GetCharacterMovement() const;
	class UCameraComponent& GetCamera() const;
	class UFPCamera& GetFPCamera() const;
	class UFPAnimator& GetAnimator() const;
	class UFPMovement& GetMovement() const;
	class UFPCombat& GetCombat() const;

	// Interface for creating player-related effects
	class AEffect* CreateEffect(const TSubclassOf<class AEffect>& aBP, const FTransform& aTransform) const;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPSceneComponentBase : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UFPSceneComponentBase();
	
	// Getters for common stuff
	class AFPController& GetController() const;
	class AFPCharacter& GetCharacter() const;
	FTransform GetActorTransform() const;
	class UCapsuleComponent& GetCapsule() const;
	class UCharacterMovementComponent& GetCharacterMovement() const;
	class UCameraComponent& GetCamera() const;
	class UFPCamera& GetFPCamera() const;
	class UFPAnimator& GetAnimator() const;
	class UFPMovement& GetMovement() const;
	class UFPCombat& GetCombat() const;

	// Interface for creating player-related effects
	class AEffect* CreateEffect(const TSubclassOf<class AEffect>& aBP, const FTransform& aTransform) const;
};
