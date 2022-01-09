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

protected:
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:

	class AFPController* GetFPController() const;
	
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input
	void MoveHorizontal(float aValue);
	void MoveVertical(float aValue);
	void LookHorizontal(float aValue);
	void LookVertical(float aValue);

	virtual void Landed(const FHitResult& aHit) override;

	class UCameraComponent* GetCamera() const;
	class UFPAnimator* GetAnimator() const;
	class UFPMovement* GetMovement() const;
	class UFPCombat* GetCombat() const;
	class UCapsuleComponent* GetWallDetection() const;

	class AHand* GetRightHand() const;
	class AHand* GetLeftHand() const;
	
	void SetHalfHeight();
	void SetFullHeight();

	class AEffect* CreateEffect(const TSubclassOf<class AEffect>& aBP, const FTransform& aTransform);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Die(const FString& anObjectName);
	
private:
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
protected:

	// Components
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UCameraComponent* myCamera;
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
