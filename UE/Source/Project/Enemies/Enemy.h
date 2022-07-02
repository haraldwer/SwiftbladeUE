#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECT_API AEnemy : public APawn
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SetSpawner(class AEnemySpawner* aSpawner);

	virtual float TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser) override;
	void Die();

	bool IsActorInDamageHitbox(AActor* anActor) const;

	class UObjectAnimator* GetObjectAnimator() const { return myObjectAnimator; }
	class UPrimitiveComponent* GetCollider() const { return Cast<UPrimitiveComponent>(myCollider); }
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTookDamage(float aDamageAmount, AActor* aDamageCauser);
	UFUNCTION(BlueprintImplementableEvent)
	void OnDied();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* myCollider;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* myMeshParent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* myAnimationParent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* myDamageHitboxParent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UEnemyBehaviour* myBehaviour;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UObjectAnimator* myObjectAnimator;

	UPROPERTY(EditDefaultsOnly)
	int myHealth = 1;

private:
	template <class T>
	bool CreateChildComponent(const FName& aName, T*& aPtr)
	{
		aPtr = CreateDefaultSubobject<T>(aName);
		CHECK_RETURN_LOG(!aPtr, "Failed to create " + aName.ToString() + " component", false);
		aPtr->SetupAttachment(myCollider);
		return true;
	}

	TWeakObjectPtr<AEnemySpawner> mySpawner; 
};
