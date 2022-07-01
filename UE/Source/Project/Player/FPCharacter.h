#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Project/Gameplay/Door.h"

#include "FPCharacter.generated.h"

UENUM()
enum EFPPostProcess
{
	PP_EYE
};

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
	class UCapsuleComponent* GetWallDetection() const	{ return myWallDetection; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class AHand* GetRightHand() const					{ return myRightHand; }
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class AHand* GetLeftHand() const					{ return myLeftHand; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class ASword* GetSword() const;
	
	// Crouch size
	void SetHalfHeight();
	void SetFullHeight();

	// Effect interface
	class AEffect* CreateEffect(const TSubclassOf<class AEffect>& aBP, const FTransform& aTransform) const;

	// Gameplay
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Die(const FString& anObjectName);
	void OnLivesChanged(int32 aNewLifeCount) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DoorOpened(ADoor* aDoor) const;

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	bool HasMagic() const;

	UFUNCTION(BlueprintCallable)
	void SetPPScalar(const EFPPostProcess aPP, const FName aName, const float aValue);

private:

	// Collision callbacks
	virtual void Landed(const FHitResult& aHit) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePPScalar(const EFPPostProcess aPP, const FName aName, const float aValue);

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
	float mySensitivity = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float myKillZ = -1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "PostProcessing")
	float myPPInterpSpeed = 10.0f;
	
private:
	
	float myFullHeight = 0.0f;
	bool myAlive = true;

	void UpdatePP(float aDT);
	
	struct PPEntry
	{
		EFPPostProcess aPP;
		FName aName;
		float aTargetValue = 0.0f;
		float aCurrentValue = 0.0f;
	};
	TArray<PPEntry> myPPEntries;
};
