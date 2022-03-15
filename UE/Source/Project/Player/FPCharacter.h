#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "FPCharacter.generated.h"

UCLASS()
class PROJECT_API AFPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AFPCharacter();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input getters
	float GetSensitivity() const						{ return mySensitivity; }
	
	// Getters
	class AFPController* GetFPController() const;
	class UFPCamera* GetFPCamera() const				{ return myFPCamera; }
	class UCameraComponent* GetCamera() const			{ return myCamera; }
	class UFPAnimator* GetAnimator() const				{ return myFPAnimator; }
	class UFPMovement* GetMovement() const	{ return myFPMovement; }
	class UFPCombat* GetCombat() const					{ return myFPCombat; }
	class UCapsuleComponent* GetWallDetection() const	{ return myWallDetection; }

	class AHand* GetRightHand() const					{ return myRightHand; }
	class AHand* GetLeftHand() const					{ return myLeftHand; }

	// Crouch size
	void SetHalfHeight();
	void SetFullHeight();

	// Effect interface
	class AEffect* CreateEffect(const TSubclassOf<class AEffect>& aBP, const FTransform& aTransform) const;

	// Gameplay
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Die(const FString& anObjectName);
	void OnRespawned();

private:

	// Collision callbacks
	virtual void Landed(const FHitResult& aHit) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
protected:

	// Components
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UCameraComponent* myCamera;
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UFPCamera* myFPCamera;
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	class UFPMovement* myFPMovement;
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	class UFPAnimator* myFPAnimator;
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	class UFPCombat* myFPCombat;

	UPROPERTY(Instanced, EditDefaultsOnly, Category="Components")
	UCapsuleComponent* myWallDetection;
	
	// Hands
	UPROPERTY(EditDefaultsOnly, Category = "Hands")
	TSubclassOf<AActor> myHandBP;
	UPROPERTY(Instanced)
	class AHand* myLeftHand;
	UPROPERTY(Instanced)
	class AHand* myRightHand;
	
	// Gameplay
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float mySensitivity = 0.5f;
	
private:
	float myFullHeight = 0.0f;
};
