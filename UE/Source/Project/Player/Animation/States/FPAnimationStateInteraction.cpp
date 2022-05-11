#include "FPAnimationStateInteraction.h"

#include "Project/Gameplay/Interaction.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Player/Combat/States/FPCombatStateInteract.h"

UClass* UFPAnimationStateInteraction::Update(float aDT)
{
	Super::Update(aDT);

	const auto defaultTrans = GetDefaultHandTransform();

	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		0.5f, 0.5f);

	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = FlipRightToLeft(hands.myRight);

	myInterpSpd = FMath::Min(
		myInterpSpd + aDT * myInterpSpdIncrease,
		myMaxInterpSpd);
	hands.myPosInterpSpd = myInterpSpd;
	hands.myRotInterpSpd = myInterpSpd; 

	if (const auto combatState = GetCombat().GetState<UFPCombatStateInteract>())
	{
		if (const auto interaction = combatState->GetInteraction())
		{
			const auto actorTrans = GetActorTransform();
			if (interaction->GetRight(hands.myRight, hands.myRightHandState))
				hands.myRight = hands.myRight * actorTrans.Inverse();
			if (interaction->GetLeft(hands.myLeft, hands.myRightHandState))
				hands.myLeft = hands.myLeft * actorTrans.Inverse();
		}
	}

	SetHands(hands, myInterpSpd >= myMaxInterpSpd);

	FFPAnimationCameraData camera;
	SetCamera(camera);

	return nullptr;
}

void UFPAnimationStateInteraction::Enter()
{
	Super::Enter();
	myInterpSpd = 0.0f;
}
