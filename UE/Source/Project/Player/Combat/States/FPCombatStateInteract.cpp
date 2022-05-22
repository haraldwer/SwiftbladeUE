#include "FPCombatStateInteract.h"

#include "FPCombatStateIdle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Gameplay/Interaction.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Animation/States/FPAnimationStateInteraction.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Utility/Tools/CustomCamera.h"

void UFPCombatStateInteract::Init()
{
	myInteraction.Reset();
}

UClass* UFPCombatStateInteract::Update(float aDT)
{	
	const auto interaction = myInteraction.Get();
	if (!interaction)
		return UFPCombatStateIdle::StaticClass();

	auto& character = GetCharacter();
	if (!IsCloseToSpecificInteraction(interaction))
	{
		interaction->EndInteract(&character);
		myInteraction.Reset();
		return UFPCombatStateIdle::StaticClass();
	}
	
	if (interaction->TickInteraction(&character))
	{
		interaction->PerformInteraction(&character);
		interaction->EndInteract(&character);
		myInteraction.Reset();
		return UFPCombatStateIdle::StaticClass();
	}
	
	return nullptr;
}

UClass* UFPCombatStateInteract::Check()
{
	return CheckConditions();
}

UClass* UFPCombatStateInteract::Input(const EFPCombatInput anAction)
{
	if (anAction == EFPCombatInput::INTERACT)
	{
		CheckConditions();
		if (const auto interaction = myInteraction.Get())
			interaction->ClickInteraction(&GetCharacter());
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

UClass* UFPCombatStateInteract::CheckConditions()
{
	if (myInteraction.IsValid())
		return StaticClass(); 
	
	UInteraction* const closest = FindClosestInteraction();
	if (!closest)
		return nullptr;

	closest->BeginInteract(&GetCharacter());
	myInteraction = closest;
	return StaticClass(); 
}

TArray<UInteraction*> UFPCombatStateInteract::FindPossibleInteractions() const
{
	TArray<AActor*> overlaps;
	TArray<UInteraction*> result; 
	GetCharacter().GetOverlappingActors(overlaps);
	if (overlaps.Num())
	{
		for (const auto& overlap : overlaps)
		{
			CHECK_CONTINUE(!overlap);
			TArray<UInteraction*> interactions;
			overlap->GetComponents(interactions);
			result.Append(interactions);
		}
	}
	return result; 
}

bool UFPCombatStateInteract::IsCloseToSpecificInteraction(const UInteraction* anInteraction) const
{
	CHECK_RETURN(!anInteraction, false);
	const auto interactions = FindPossibleInteractions();
	CHECK_RETURN(!interactions.Contains(anInteraction), false);
	const auto diff = anInteraction->GetComponentLocation() - GetCharacter().GetTransform().GetLocation();
	const auto dot = diff.GetSafeNormal().Dot(GetCamera().GetForwardVector());
	CHECK_RETURN(dot < myAcceptableDot, false);
	return true; 
}

UInteraction* UFPCombatStateInteract::FindClosestInteraction() const
{
	const auto trans = GetCharacter().GetTransform();
	const auto loc = trans.GetLocation();
	const auto forward = GetCamera().GetForwardVector();	
	auto interactions = FindPossibleInteractions();
	float dist = 99999999999999999.0f;
	UInteraction* current = nullptr;
	for (const auto interaction : interactions)
	{
		CHECK_CONTINUE(interaction->HasFinished());
		const auto intLoc = interaction->GetComponentLocation();
		const auto diff = intLoc - loc;
		const float thisDist = diff.SquaredLength();
		CHECK_CONTINUE(thisDist >= dist);
		CHECK_CONTINUE(diff.GetSafeNormal().Dot(forward) < myAcceptableDot);
		current = interaction;
		dist = thisDist;
	}
	return current;
}
