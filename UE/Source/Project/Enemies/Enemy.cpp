#include "Enemy.h"

#include "Behaviour/EnemyBehaviour.h"
#include "EnemyManager.h"
#include "Components/StaticMeshComponent.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/Animation/ObjectAnimator.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	myBehaviour = CreateDefaultSubobject<UEnemyBehaviour>("EnemyBehaviour");
	CHECK_RETURN_LOG(!myBehaviour, "Failed to create EnemyBehaviour component");
	
	CreateChildComponent<USceneComponent>("MeshParent", myMeshParent);
	CreateChildComponent<USceneComponent>("AnimationParent", myAnimationParent);
	CreateChildComponent<USceneComponent>("DamageHitboxParent", myDamageHitboxParent);

	myObjectAnimator = CreateDefaultSubobject<UObjectAnimator>("ObjectAnimator");
	CHECK_RETURN_LOG(!myObjectAnimator, "Failed to create ObjectAnimator component");
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	UMainSingelton::GetEnemyManager().AddEnemy(this);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::SetSpawner(AEnemySpawner* aSpawner)
{
	mySpawner = aSpawner;
}

float AEnemy::TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser)
{
	LOG("Enemy took damage " + FString::SanitizeFloat(aDamageAmount));
	myHealth -= FMath::RoundToInt(aDamageAmount);
	Super::TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser);
	OnTookDamage(aDamageAmount, aDamageCauser);
	if (myHealth <= 0)
		Die();
	return aDamageAmount;
}

void AEnemy::Die()
{
	LOG("Enemy died");
	OnDied();
	UMainSingelton::GetEnemyManager().RemoveEnemy(this);
	if (mySpawner.IsValid())
		mySpawner->RemoveEnemy(this);
	// TODO: Death animation
	Destroy();
}

bool AEnemy::IsActorInDamageHitbox(AActor* anActor) const
{
	if (!anActor)
		return false;
	for (const auto& hitbox : myDamageHitboxParent->GetAttachChildren())
	{
		UShapeComponent* shape = Cast<UShapeComponent>(hitbox);
		CHECK_CONTINUE_LOG(!shape, "Hitbox is not a shape");
		TArray<AActor*> actors;
		shape->GetOverlappingActors(actors, anActor->StaticClass());
		for (const auto& overlap : actors)
			if (overlap == anActor)
				return true;
	}
	return false;
}
