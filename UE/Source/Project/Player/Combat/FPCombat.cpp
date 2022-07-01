#include "FPCombat.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateBase.h"
#include "Project/Player/Movement/FPMovement.h"
#include "States/FPCombatStateBase.h"
#include "States/FPCombatStateIdle.h"
#include "States/FPCombatStateNoSword.h"
#include "UObject/UObjectIterator.h"


UFPCombat::UFPCombat()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPCombat::BeginPlay()
{
	Super::BeginPlay();

	const auto& character = GetCharacter();
	AddTickPrerequisiteComponent(Cast<UActorComponent>(character.GetMovement()));
}

void UFPCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (mySwordFirstTick)
	{
		mySwordFirstTick = false; 
		if (const auto sword = mySword.Get())
		{
		   	// Set crystals
		   	if (const auto controller = GetCharacter().GetFPController())
		   		sword->SetCrystalsActive(controller->GetRemainingLives(), false);

			// Set position
			if (const auto hand = GetCharacter().GetRightHand())
				sword->SetActorTransform(hand->GetActorTransform());
		}
	}
	
	TryOverrideAnimation();
}

UClass* UFPCombat::GetDefaultStateType()
{
	return UFPCombatStateNoSword::StaticClass();
}

bool UFPCombat::SetStatePtr(UStateBase* aState)
{
	// Call base function
	CHECK_RETURN(!Super::SetStatePtr(aState), false);

	TryOverrideAnimation();

	return true;
}

void UFPCombat::PickupSword()
{
	const auto sword = mySword.Get();
	CHECK_RETURN_LOG(!sword, "Sword ptr not set");
	LOG("PickupSword");
	sword->SetPlayer(&GetCharacter());
	SetState<UFPCombatStateIdle>();
}

void UFPCombat::ReturnSword()
{
	const auto sword = mySword.Get();
	CHECK_RETURN_LOG(!sword, "No sword to return");
	sword->Return();
	mySword.Reset();
	SetState<UFPCombatStateNoSword>();
}

void UFPCombat::SetHasSword(const bool aValue)
{
	if (!aValue)
	{
		ReturnSword();
		return;
	}

	auto& character = GetCharacter();
	if (!mySword.IsValid())
	{
		// Find sword in world
		for (TObjectIterator<ASword> itr; itr; ++itr)
		{
			const auto ptr = *itr;
			CHECK_CONTINUE(!ptr);
			CHECK_CONTINUE(ptr->GetWorld() != GetWorld());
			mySword = ptr;
			mySwordFirstTick = true;
			break;
		}
	}

	if (!mySword.IsValid())
		if (const auto swordBP = mySwordBP.Get())
			mySword = Cast<ASword>(GetWorld()->SpawnActor(swordBP));
	
	if (const auto sword = mySword.Get())
	{
		sword->SetPlayer(&character);
		SetState<UFPCombatStateIdle>();
	}
}

bool UFPCombat::HasSword() const
{
	const auto currentState = GetCurrentState();
	return currentState && !currentState->IsA(UFPCombatStateNoSword::StaticClass());
}

void UFPCombat::SetSword(ASword* aSword)
{
	mySword = aSword;
}

ASword* UFPCombat::GetSword() const
{
	return mySword.Get();
}

bool UFPCombat::TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser) const
{
	if (const auto state = GetCurrentState())
		return state->TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser);
	return false;
}

AFPCharacter& UFPCombat::GetCharacter() const
{
	return *Cast<AFPCharacter>(GetOwner());
}

void UFPCombat::Input(const EFPCombatInput anInput)
{
	UStateBase* nextState = nullptr;
	for (auto& it : myStates)
		if (const auto ptr = Cast<UFPCombatStateBase>(it.Get()))
			if (const auto newStateType = ptr->Input(anInput))
				if (const auto newState = GetState(newStateType))
					nextState = newState;
	if (nextState)
		SetStatePtr(nextState);
}

void UFPCombat::TryOverrideAnimation() const
{
	const auto character = Cast<AFPCharacter>(GetOwner());
	CHECK_RETURN_LOG(!character, "No character");
	UFPAnimatorNew* animator = character->GetAnimator();
	CHECK_RETURN_LOG(!animator, "No animator");
	const auto* state = Cast<UFPCombatStateBase>(GetCurrentState());
	CHECK_RETURN(!state);
	const auto animation = state->GetAnimation();
	const auto animationClass = animation.Get();
	CHECK_RETURN(!animationClass);
	animator->TryOverrideState(animator->GetState(animationClass));
}

