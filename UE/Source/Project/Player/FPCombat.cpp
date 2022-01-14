#include "FPCombat.h"

#include "FPCharacter.h"
#include "FPController.h"
#include "Actors/Hand.h"
#include "Actors/Sword.h"
#include "Animation/FPAnimator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/CustomGameMode.h"
#include "Project/Utility.h"
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
	
	UpdateStrike();
	UpdateSword(DeltaTime);
	UpdateTransforms(DeltaTime);
}

void UFPCombat::PickupSword()
{
	CHECK_RETURN_LOG(!mySword, "Sword ptr not set");
	LOG("PickupSword");
	mySword->SetPlayer(&GetCharacter());
	mySword->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	myState = EFPCombatState::IDLE;
}

bool UFPCombat::GetHasSword() const
{
	return myState != EFPCombatState::NO_SWORD;
}

void UFPCombat::Strike()
{
	LOG("Strike pressed");
	
	myStrikeTime = 0.0f;
	if (mySword)
		myState = EFPCombatState::READY;
		
	myAnimIndex = myDefaultAnimIndex != -1 ?
			myDefaultAnimIndex : FMath::RandRange(0, myAnimations.Num() - 1);
	
	if(!mySword)
	{
		TArray<AActor*> actors;
		GetCapsule().GetOverlappingActors(actors, ASword::StaticClass());
		for (const auto& it : actors)
		{
			mySword = Cast<ASword>(it);
			break;
		}
	}
}

void UFPCombat::UpdateStrike()
{
	CHECK_RETURN(myState != EFPCombatState::READY);
	CHECK_RETURN(!mySword);
	
	EFPCombatState newState = EFPCombatState::READY; 
	
	// Checkpoints
	for (const auto& c : mySword->GetOverlaps(ACheckpoint::StaticClass()))
	{
		HitCheckpoint(c);
		newState = EFPCombatState::DEFLECT;
		break;
	}

	// Enemies
	for (const auto& e : mySword->GetOverlaps(AEnemy::StaticClass()))
	{
		HitEnemy(e);
		newState = EFPCombatState::STRIKE;
		break;
	}

	if(newState != myState)
	{
		UpdateSword(0.0f);
		SetTransforms();
		myStrikeTime = 0.0f;
		myState = newState;
	}
}

void UFPCombat::UpdateSword(float aDT)
{	
	myStrikeTime += aDT;
	
	if (!mySword)
		myState = EFPCombatState::NO_SWORD;
	
	switch (myState)
	{
	case EFPCombatState::NO_SWORD:
		if (myStrikeTime < 0.2f)
		{
			// Grip sword with right hand
			if (mySword)
			{
				myTargetTrans = FTransform(
					GetActorTransform().InverseTransformPosition(
						mySword->GetActorLocation()));
				UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::INTRO);
			}
			else
			{
				myTargetTrans = FTransform();
				myTargetTrans.SetLocation(FVector(50, 20, 30));
				myTargetTrans.SetRotation(FQuat::MakeFromEuler(FVector(-10, 40, 0)));
			}
			myUseBothHands = false;
			myUseWeight = true;
			myTargetLocationWeight = 1.0f;
			myTargetRotationWeight = 1.0f;
		}
		else
		{
			myUseWeight = false;
			if (mySword)
				PickupSword();
		}
		break;
		
	default:
	case EFPCombatState::IDLE:
		{
			myUseWeight = true;
			myUseBothHands = false;
			myTargetLocationWeight = 0.1f;
			myTargetRotationWeight = 0.8f;
			myTargetTrans = myIdleSwordTransform;
			break;
		}
		
	case EFPCombatState::READY:
		{
			if (!myAnimations.IsValidIndex(myAnimIndex))
				break;
			myUseWeight = true;
			myUseBothHands = true;
			myTargetLocationWeight = 1.0f;
			myTargetRotationWeight = 1.0f;
			myTargetTrans = myAnimations[myAnimIndex].myReadyTransform;

			if (myStrikeTime > myReadyDuration)
				myState = EFPCombatState::IDLE;
			break;
		}
		
	case EFPCombatState::STRIKE:
		{
			if (!myAnimations.IsValidIndex(myAnimIndex))
				break;
			myUseWeight = true;
			myUseBothHands = true;
			const float part = (myStrikeTime / myStrikeDuration);
			const float weight = 1.0f - part * part; 
			myTargetLocationWeight = weight;
			myTargetRotationWeight = weight;
			myTargetTrans = myAnimations[myAnimIndex].myStrikeTransform;
			if (myStrikeTime > myStrikeDuration)
				myState = EFPCombatState::IDLE;
			break;
		}
		
	case EFPCombatState::DEFLECT:
		{
			if (!myAnimations.IsValidIndex(myAnimIndex))
				break;
			myUseWeight = true;
			myUseBothHands = true;
			const float part = (myStrikeTime / myDeflectDuration);
			const float weight = 1.0f - part * part;
			myTargetLocationWeight = weight;
			myTargetRotationWeight = weight;
			myTargetTrans = myAnimations[myAnimIndex].myDeflectTransform;
			if (myStrikeTime > myDeflectDuration)
				myState = EFPCombatState::IDLE;
			break;
		}
	}
}

void UFPCombat::SetTransforms()
{
	const auto& character = GetCharacter();
	const auto& animator = GetAnimator();
	
	const auto right = character.GetRightHand();
	const auto left = character.GetLeftHand();
	if (!right || !left)
		return;

	const float locationWeight = FMath::Max(myTargetLocationWeight, animator.GetSwordPart()) * myUseWeight;
	const float rotationWeight = FMath::Max(myTargetRotationWeight, animator.GetSwordPart()) * myUseWeight;

	myLocationWeight = locationWeight;
	myRotationWeight = rotationWeight;
	myTrans = myTargetTrans;
	
	const auto rTrans =
		LerpTransWeight(animator.GetRight(), myTrans, myLocationWeight, myRotationWeight);
	
	const auto lTrans =
		myUseBothHands ?
			LerpTransWeight(animator.GetLeft(), myTrans, myLocationWeight, myRotationWeight) :
			animator.GetLeft();
	
	right->SetActorRelativeTransform(rTrans);
	left->SetActorRelativeTransform(lTrans);
	
	if(mySword && myState != EFPCombatState::NO_SWORD)
		mySword->SetActorRelativeTransform(rTrans);
	
}

void UFPCombat::UpdateTransforms(float aDT)
{
	const auto& character = GetCharacter();
	const auto& animator = GetAnimator();
	
	const auto right = character.GetRightHand();
	const auto left = character.GetLeftHand();
	if (!right || !left)
		return;

	if (!myTargetTrans.IsValid() || myTargetTrans.GetLocation().Size() > 500.0f)
	{
		LOG("Invalid target transform");
		return;
	}
	
	const float locationWeight = FMath::Max(myTargetLocationWeight, animator.GetSwordPart()) * myUseWeight;
	const float rotationWeight = FMath::Max(myTargetRotationWeight, animator.GetSwordPart()) * myUseWeight;

	myLocationWeight = FMath::FInterpTo(myLocationWeight, locationWeight, aDT, mySmoothing);
	myRotationWeight = FMath::FInterpTo(myRotationWeight, rotationWeight, aDT, mySmoothing);

	// Sword transform
	myTrans = LerpTransDelta(myTrans, myTargetTrans, aDT, mySmoothing);
	if (!myTrans.IsValid() || myTrans.GetLocation().Size() > 500.0f)
	{
		LOG("Invalid transform");
		return;
	}

	if (!animator.GetRight().IsValid())
	{
		LOG("Animator right invalid");
	}

	// Right hand transform, lerp between animation and sword trans
	const auto rTrans =
		LerpTransWeight(animator.GetRight(), myTrans, myLocationWeight, myRotationWeight);

	// Sword left hand transform 
	auto leftTarget = myTrans;
	leftTarget.SetLocation(myTrans.TransformPosition(FVector(0, 0, 5)));

	// Left hand weight, Use both hands?
	myLeftWeight = FMath::FInterpTo(myLeftWeight, myUseBothHands, aDT, mySmoothing);

	// Left hand transform, lerp between animation and sword trans
	const auto lTrans =
		LerpTransWeight(animator.GetLeft(), leftTarget, myLocationWeight * myLeftWeight, myRotationWeight * myLeftWeight);

	if (!rTrans.IsValid())
	{
		LOG("Invalid right");
		return;
	}

	if (rTrans.GetLocation().Size() > 500.0f)
	{
		LOG("Weird right position");
		return;
	}

	if (!lTrans.IsValid() || lTrans.GetLocation().Size() > 500.0f)
	{
		LOG("Invalid left");
		return;
	}
	
	right->SetActorRelativeTransform(rTrans);
	left->SetActorRelativeTransform(lTrans);
	left->SetActorRelativeScale3D(FVector(1, -1, 1));
	
	right->SetIsOpen(!GetHasSword());
	left->SetIsOpen(!GetHasSword() || !myUseBothHands);
	
	if(mySword && myState != EFPCombatState::NO_SWORD)
		mySword->SetActorRelativeTransform(rTrans);
}

ASword* UFPCombat::GetSword() const
{
	return mySword;
}

void UFPCombat::HitCheckpoint(AActor* aCheckpoint) const
{
	GetController().SetCheckpoint(aCheckpoint);
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
}
