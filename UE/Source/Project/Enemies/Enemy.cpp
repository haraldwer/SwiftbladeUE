#include "Enemy.h"

#include "EnemyManager.h"
#include "Behaviour/EnemyBehaviour.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/Animation/ObjectAnimator.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	myCollider = CreateDefaultSubobject<USceneComponent>("Collider");
	CHECK_RETURN_LOG(!myCollider, "Failed to create collider component");
	RootComponent = myCollider;
	
	CreateChildComponent<USceneComponent>("MeshParent", myMeshParent);
	CreateChildComponent<USceneComponent>("AnimationParent", myAnimationParent);
	CreateChildComponent<USceneComponent>("DamageHitboxParent", myDamageHitboxParent);

	myBehaviour = CreateDefaultSubobject<UEnemyBehaviour>("EnemyBehaviour");
	CHECK_RETURN_LOG(!myBehaviour, "Failed to create EnemyBehaviour component");
	
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

float AEnemy::TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser)
{
	LOG(GetName() + " enemy took damage " + FString::SanitizeFloat(aDamageAmount));
	myHealth -= FMath::RoundToInt(aDamageAmount);
	Super::TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser);
	if (myBehaviour)
		myBehaviour->OnTookDamage(aDamageAmount, aDamageCauser);
	OnTookDamage(aDamageAmount, aDamageCauser);
	if (myHealth <= 0)
		Die();
	return aDamageAmount;
}

void AEnemy::Die()
{
	LOG(GetName() + " enemy died ");
	if (myBehaviour)
		myBehaviour->OnDied();
	OnDied();
	UMainSingelton::GetEnemyManager().RemoveEnemy(this);
	Destroy();
}

bool AEnemy::IsActorInDamageHitbox(const AActor* anActor) const
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
