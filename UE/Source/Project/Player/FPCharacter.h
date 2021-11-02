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

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input
	void MoveHorizontal(float aValue);
	void MoveVertical(float aValue);
	void LookHorizontal(float aValue);
	void LookVertical(float aValue);

	virtual void Landed(const FHitResult& aHit) override;

	void SetCheckpoint(AActor* aActor);
	
	class UCameraComponent* GetCamera() const;
	class UFPAnimator* GetAnimator() const;
	class UFPMovement* GetMovement() const;
	class UFPCombat* GetCombat() const;

	class AHand* GetRightHand() const;
	class AHand* GetLeftHand() const;
	
	void SetHalfHeight();
	void SetFullHeight();

private:
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Die(FString anObjectName);

protected:

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UCameraComponent* myCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UFPAnimator* myAnimator;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UFPMovement* myMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UFPCombat* myCombat;

	// Hands
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hands")
	TSubclassOf<AActor> myHandBP;
	UPROPERTY()
	class AHand* myLeftHand;
	UPROPERTY()
	class AHand* myRightHand;

	// Gameplay
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int myRespawns = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float mySensitivity = 0.5f;
	
private:
	float myFullHeight = 0.0f;
	FVector myCheckpointLocation;
	bool myHasCheckpoint = true;
	int myRespawnCount = 0;
	bool myDiedThisFrame = false;
};
