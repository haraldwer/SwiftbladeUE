#include "FPCombat.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/UI/Prompts/PromptManager.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Math/LerpTransform.h"

UFPCombat::UFPCombat()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UFPCombat::BeginPlay()
{
	Super::BeginPlay();
}

void UFPCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	Update(DeltaTime);
}

void UFPCombat::PickupSword()
{
	CHECK_RETURN_LOG(!mySword, "Sword ptr not set");
	LOG("PickupSword");
	mySword->SetPlayer(&GetCharacter());
	myState = EFPCombatState::IDLE;
}

void UFPCombat::ReturnSword()
{
	CHECK_RETURN_LOG(!mySword, "No sword to return");
	mySword->Return();
	mySword = nullptr;
	myState = EFPCombatState::IDLE;
}

void UFPCombat::SetHasSword(const bool aValue)
{
	if (!aValue)
	{
		ReturnSword();
		return;
	}

	for (TObjectIterator<ASword> itr; itr; ++itr)
	{
		const auto ptr = *itr;
		CHECK_CONTINUE(!ptr);
		CHECK_CONTINUE(ptr->GetWorld() != GetWorld());
		mySword = ptr;
		mySword->SetPlayer(&GetCharacter());
		myState = EFPCombatState::IDLE;
		return;
	}

	LOG("No sword in world"); 
}

bool UFPCombat::GetHasSword() const
{
	return myState != EFPCombatState::NO_SWORD;
}

void UFPCombat::Strike()
{
	LOG("Strike pressed");
	CHECK_RETURN_LOG(myTimer < myStrikeDuration, "Already in strike");

	if(!mySword)
	{
		myState = EFPCombatState::NO_SWORD;
		DoInteract();
		return;
	}

	DoStrike();
	
	myTimer = 0.0f;
	myState = EFPCombatState::STRIKE;
	SelectAnim(myStrikeAnimations.Num());
}

void UFPCombat::Block()
{
	CHECK_RETURN_LOG(myTimer < myBlockDuration, "Already in block");

	DoBlock();
	
	myTimer = 0.0f;
	myState = EFPCombatState::BLOCK;	
	SelectAnim(myBlockAnimations.Num());
}

void UFPCombat::Interact()
{
	if (GetHasSword())
		DoInteract();
}

void UFPCombat::Update(float aDT)
{	
	myTimer += aDT;
	
	if (!mySword)
		myState = EFPCombatState::NO_SWORD;
	
	switch (myState)
	{
	case EFPCombatState::NO_SWORD:
		{
			if (myTimer < 0.2f)
			{
				// Grip sword with right hand
				if (mySword)
				{
					// Move hand towards sword position
					UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::INTRO);
				}
			}
			else
			{
				if (mySword)
					PickupSword();
			}
			break;
		}
		
	default:
	case EFPCombatState::IDLE:
		{
			break;
		}
		
	case EFPCombatState::STRIKE:
		{
			DoStrike();
			if (myTimer > myStrikeDuration)
				myState = EFPCombatState::IDLE;
			break;
		}
		
	case EFPCombatState::BLOCK:
		{
			DoBlock();
			if (myTimer > myBlockDuration)
				myState = EFPCombatState::IDLE;
			break;
		}
	}
}

ASword* UFPCombat::GetSword() const
{
	return mySword;
}

void UFPCombat::DoStrike()
{
	CHECK_RETURN(myTimer > myPerformStrikeDuration);

	// Checkpoints
	for (const auto& c : mySword->GetOverlaps(ACheckpoint::StaticClass()))
	{
		HitCheckpoint(c);
		break;
	}

	// Enemies
	for (const auto& e : mySword->GetOverlaps(AEnemy::StaticClass()))
	{
		HitEnemy(e);
		break;
	}
}

void UFPCombat::DoBlock()
{
	CHECK_RETURN(myTimer > myPerformBlockDuration);
	
}

void UFPCombat::DoInteract()
{
	CHECK_RETURN_LOG(myTimer < myInteractDuration, "Already in interact"); 
	myTimer = 0.0f;
	TArray<AActor*> actors;
	GetCapsule().GetOverlappingActors(actors, ASword::StaticClass());
	for (const auto& it : actors)
	{
		mySword = Cast<ASword>(it);
		break;
	}
}

void UFPCombat::HitCheckpoint(AActor* aCheckpoint) const
{
	const auto checkpoint = Cast<ACheckpoint>(aCheckpoint);
	CHECK_RETURN_LOG(!checkpoint, "Not a checkpoint")
	if (GetController().SetCheckpoint(checkpoint))
	{
		FTransform trans = mySword->GetActorTransform();
		trans.SetLocation(trans.GetLocation() + mySword->GetActorForwardVector() * 100);
		CreateEffect(myHitEffectBP, trans);		
	}
}

void UFPCombat::HitEnemy(AActor* anEnemy)
{
	const auto sword = GetSword();
	CHECK_RETURN_LOG(!sword, "Sword nullptr");
	UGameplayStatics::ApplyDamage(
		anEnemy,
		1.0f,
		&GetController(),
		sword,
		UDamageType::StaticClass());

	FTransform trans = mySword->GetActorTransform();
	trans.SetLocation(trans.GetLocation() + mySword->GetActorForwardVector() * 100);
	CreateEffect(myHitEffectBP, trans); 
}

inline void UFPCombat::SelectAnim(const int aNum)
{
	myAnimIndex = FMath::RandRange(0, aNum - 1);
	if (myAnimIndex == myPreviousAnimIndex)
	{
		myAnimIndex++;
		if (myAnimIndex == aNum)
			myAnimIndex = 0;
	}
	myPreviousAnimIndex = myAnimIndex;
}
