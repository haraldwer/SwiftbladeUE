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
	void SetSpawner(class AEnemySpawner* aSpawner) { mySpawner = aSpawner; }

	virtual float TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser) override;
	void Die();

	bool IsActorInDamageHitbox(AActor* anActor);
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTookDamage(float aDamageAmount, AActor* aDamageCauser);
	UFUNCTION(BlueprintImplementableEvent)
	void OnDied();
	
	UPROPERTY(EditDefaultsOnly)
	class UEnemyBehaviour* myBehaviour;
	UPROPERTY(EditDefaultsOnly)
	class UEnemyAnimator* myAnimator;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* myMesh;
	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* myDamageHitboxParent;

	UPROPERTY(EditDefaultsOnly)
	int myHealth = 1;

private:
	UPROPERTY()
	class AEnemySpawner* mySpawner; 
};
