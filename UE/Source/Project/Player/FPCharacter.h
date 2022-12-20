#pragma once

#include "CoreMinimal.h"
#include "FPController.h"
#include "GameFramework/Character.h"
#include "FPCharacter.generated.h"

class UFPToombstone;
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

	void OnStateLoaded(const FFPControllerState& aState) const;

	// Input getters
	float GetSensitivity() const						{ return mySensitivity; }
	
	// Getters
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class AFPController* GetFPController() const;
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UCustomCamera* GetCamera() const				{ return myCamera; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UFPCamera* GetFPCamera() const				{ return myFPCamera; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UFPAnimatorNew* GetAnimator() const			{ return myFPAnimator; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UFPMovement* GetMovement() const				{ return myFPMovement; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UFPCombat* GetCombat() const					{ return myFPCombat; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UFPTime* GetTime() const						{ return myFPTime; }
	
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UFPPostProcessing* GetPostProcessing() const	{ return myFPPP; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UCapsuleComponent* GetWallDetection() const	{ return myWallDetection; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class USphereComponent* GetInteractionCollider() const { return myInteractionCollider; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class AHand* GetRightHand() const					{ return myRightHand; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class AHand* GetLeftHand() const					{ return myLeftHand; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class ASword* GetSword() const;

	// Effect interface
	class AEffect* CreateEffect(const TSubclassOf<class AEffect>& aBP, const FTransform& aTransform) const;

	// Gameplay
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Die(const FString& anObjectName);
	void OnLivesChanged(int32 aNewLifeCount) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DoorOpened(class ADoor* aDoor) const;

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	bool HasMagic() const;

private:

	virtual void OnStartCrouch(float aHalfHeightAdjust, float aScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float aHalfHeightAdjust, float aScaledHalfHeightAdjust) override;

	// Collision callbacks
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Landed(const FHitResult& aHit) override;
	
protected:

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UCustomCamera* myCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UFPCamera* myFPCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UFPMovement* myFPMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UFPAnimatorNew* myFPAnimator;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UFPCombat* myFPCombat;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UFPTime* myFPTime;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UFPPostProcessing* myFPPP;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UFPToombstone* myFPToombstone;

	UPROPERTY(EditDefaultsOnly, Category="Components")
	UCapsuleComponent* myWallDetection;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	USphereComponent* myInteractionCollider;
	
	// Hands
	UPROPERTY(EditDefaultsOnly, Category = "Hands")
	TSubclassOf<AActor> myHandBP;
	UPROPERTY(Transient)
	class AHand* myLeftHand;
	UPROPERTY(Transient)
	class AHand* myRightHand;
	
	// Gameplay
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float mySensitivity = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float myKillZ = -1000.0f;
	
private:
	
	float myFullHeight = 0.0f;
	bool myAlive = true;
	
};
