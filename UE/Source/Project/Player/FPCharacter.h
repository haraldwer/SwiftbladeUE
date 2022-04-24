#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Project/Gameplay/Door.h"

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
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class AFPController* GetFPController() const;
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class UCustomCamera* GetCamera() const				{ return myCamera; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class UFPCamera* GetFPCamera() const				{ return myFPCamera; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class UFPAnimatorNew* GetAnimator() const				{ return myFPAnimator; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class UFPMovement* GetMovement() const				{ return myFPMovement; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class UFPCombat* GetCombat() const					{ return myFPCombat; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class UCapsuleComponent* GetWallDetection() const	{ return myWallDetection; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class AHand* GetRightHand() const					{ return myRightHand; }
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
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
	
	void DoorOpened(ADoor* aDoor) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool HasMagic() const;

private:

	// Collision callbacks
	virtual void Landed(const FHitResult& aHit) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
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

	UPROPERTY(EditDefaultsOnly, Category="Components")
	UCapsuleComponent* myWallDetection;
	
	// Hands
	UPROPERTY(EditDefaultsOnly, Category = "Hands")
	TSubclassOf<AActor> myHandBP;
	UPROPERTY(Transient)
	class AHand* myLeftHand;
	UPROPERTY(Transient)
	class AHand* myRightHand;
	
	// Gameplay
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float mySensitivity = 0.5f;
	
private:
	float myFullHeight = 0.0f;
};
