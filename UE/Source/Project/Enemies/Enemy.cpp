#include "Enemy.h"

#include "EnemyAnimator.h"
#include "EnemyBehaviour.h"
#include "EnemyManager.h"
#include "Project/Utility/MainSingelton.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	
	myBehaviour = CreateDefaultSubobject<UEnemyBehaviour>("EnemyBehaviour");
	CHECK_RETURN_LOG(!myBehaviour, "Failed to create EnemyBehaviour component");
	myAnimator = CreateDefaultSubobject<UEnemyAnimator>("EnemyAnimator");
	CHECK_RETURN_LOG(!myAnimator, "Failed to create EnemyAnimator component");
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
	myHealth -= FMath::RoundToInt(aDamageAmount);
	Super::TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser);
	OnTookDamage(aDamageAmount, aDamageCauser);
	if (myHealth <= 0)
		Die();
	return aDamageAmount;
}

void AEnemy::Die()
{	
	OnDied();
	UMainSingelton::GetEnemyManager().RemoveEnemy(this);
	if (mySpawner)
		mySpawner->RemoveEnemy(this);
	// TODO: Death animation
	Destroy();
}
