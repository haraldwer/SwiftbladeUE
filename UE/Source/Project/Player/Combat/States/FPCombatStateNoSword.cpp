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
			combat.SetHasSword(true);
			return UFPCombatStateIdle::StaticClass();
		}
	}
	
	return nullptr;
}

UClass* UFPCombatStateNoSword::Input(const EFPCombatInput anAction)
{
	CHECK_RETURN(GetCurrentState() != this, nullptr);
	CHECK_RETURN(anAction != EFPCombatInput::INTERACT, nullptr);

	// Is sword nearby? 
	TArray<AActor*> actors;
	GetCapsule().GetOverlappingActors(actors, ASword::StaticClass());
	for (const auto& it : actors)
	{
		GetCombat().SetSword(Cast<ASword>(it));
		return StaticClass();
	}
	
	return nullptr;
}

void UFPCombatStateNoSword::Enter()
{
	myTimer = 0.0f;
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateNoSword::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
