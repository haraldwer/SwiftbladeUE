
#include "Interaction.h"

UInteractionHand::UInteractionHand()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UInteraction::UInteraction()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteraction::BeginPlay()
{
	Super::BeginPlay();
	myInteracting = false; 
	
	TArray<USceneComponent*> children; 
	GetChildrenComponents(false, children);
	for (const auto& child : children)
		if (const auto hand = Cast<UInteractionHand>(child))
			myHandComponents.Add(hand);
	myHandComponents.Sort([](const UInteractionHand& aFirst, const UInteractionHand& aSecond)
		{ return aFirst.GetName() < aSecond.GetName(); });
}

void UInteraction::BeginInteract(AFPCharacter* aCharacter)
{
	myInteracting = true; 
	if (myOnBeginInteract.IsBound())
		myOnBeginInteract.Broadcast(aCharacter);
}

void UInteraction::EndInteract(AFPCharacter* aCharacter)
{
	myInteracting = false; 
	if (myOnEndInteract.IsBound())
		myOnEndInteract.Broadcast(aCharacter);
}

bool UInteraction::TickInteraction(AFPCharacter* aCharacter)
{
	if (myOnTickInteraction.IsBound())
		myOnTickInteraction.Broadcast(aCharacter);

	return HasFinished(); 
}

void UInteraction::PerformInteraction(AFPCharacter* aCharacter)
{
	if (myOnPerformInteraction.IsBound())
		myOnPerformInteraction.Broadcast(aCharacter);
}

void UInteraction::ClickInteraction(AFPCharacter* aCharacter)
{
	if (myOnClickInteraction.IsBound())
		myOnClickInteraction.Broadcast(aCharacter);
}

void UInteraction::FinishInteraction()
{
	myFinished = true;
}

bool UInteraction::GetHand(int32 index, FTransform& aTrans, EHandState& aHandState)
{
	CHECK_RETURN(!myHandComponents.IsValidIndex(index), false);
	const auto hand = myHandComponents[index].Get();
	CHECK_RETURN(!hand, false)
	aTrans = hand->GetComponentTransform();
	aHandState = hand->GetHandState();
	return true;
}
