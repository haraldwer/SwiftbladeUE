#include "FPCombatStateNoSword.h"

#include "FPCombatStateIdle.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Animation/States/FPAnimationStatePickupSword.h"
#include "Project/Player/Combat/FPCombat.h"

UClass* UFPCombatStateNoSword::Update(float aDT)
{
	auto& combat = GetCombat();
	if (combat.HasSword())
		return UFPCombatStateIdle::StaticClass();

	if (auto sword = GetSword())
	{
		auto& animator = GetAnimator();
		animator.TryOverrideState(animator.GetState<UFPAnimationStatePickupSword>());
		myTimer += aDT;
		if (myTimer > mySwordPickupTimer)
		{
			// Pickup sword
			combat.PickupSword(); 
			return UFPCombatStateIdle::StaticClass();
		}
	}
	
	return nullptr;
}

UClass* UFPCombatStateNoSword::Input(const EFPCombatInput anAction)
{
	// Only when in this state
	CHECK_RETURN(GetCurrentState() != this, nullptr);

	// Block all other inputs
	CHECK_RETURN(anAction != EFPCombatInput::INTERACT, StaticClass());

	// Is sword nearby? 
	TArray<AActor*> actors;
	GetCapsule().GetOverlappingActors(actors, ASword::StaticClass());
	for (const auto& it : actors)
	{
		GetCombat().SetSword(Cast<ASword>(it));
		break;
	}
	
	return StaticClass();
}

void UFPCombatStateNoSword::Enter()
{
	myTimer = 0.0f;
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateNoSword::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
