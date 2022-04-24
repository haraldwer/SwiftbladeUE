#include "FPCombatStateInteract.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Animation/States/FPAnimationStateInteraction.h"

UClass* UFPCombatStateInteract::Update(float aDT)
{	
	LOG("Interact");
	return nullptr;
}

UClass* UFPCombatStateInteract::Input(const EFPCombatInput anAction)
{
	if (anAction == EFPCombatInput::INTERACT)
	{
		// Are there any close interactions?

		
	}
	return nullptr;
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateInteract::GetAnimation() const
{
	return UFPAnimationStateInteraction::StaticClass();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateInteract::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
