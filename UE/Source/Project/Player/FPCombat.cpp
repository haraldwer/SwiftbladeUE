#include "FPCombat.h"

#include "FPCharacter.h"
#include "Actors/Hand.h"
#include "Actors/Sword.h"
#include "Animation/FPAnimator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Gameplay/Checkpoint.h"

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
	if (!mySword)
		return;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "PickupSword");
	mySword->SetPlayer(GetFPCharacter());
	mySword->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	myState = EFPCombatState::IDLE;
}

bool UFPCombat::GetHasSword() const
{
	return myState != EFPCombatState::NO_SWORD;
}

void UFPCombat::Strike()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "StrikePressed");

	const auto character = GetFPCharacter();
	if (!character)
		return;

	myStrikeTime = 0.0f;
	UpdateStrike();
	
	if(!mySword)
	{
		TArray<AActor*> actors;
		character->GetCapsuleComponent()->GetOverlappingActors(actors, ASword::StaticClass());
		for (const auto& it : actors)
		{
			mySword = Cast<ASword>(it);
			break;
		}
	}
}

void UFPCombat::UpdateStrike()
{
	if (myState == EFPCombatState::NO_SWORD)
		return;

	if (!mySword)
		return;
	
	const auto character = GetFPCharacter();
	if (!character)
		return;
	
	// Checkpoints
	for (auto& c : mySword->GetOverlaps(ACheckpoint::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Checkpoint");
		character->SetCheckpoint(c);
		break;
	}

	// Enemies
	for (auto& c : mySword->GetOverlaps(AEnemy::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Enemy");
		break;
	}
}

void UFPCombat::UpdateSword(float aDT)
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;
	
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
				myTargetTrans = character->GetTransform().GetRelativeTransform(mySword->GetTransform());
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Extending");
				
				FVector p;
				p.X = 100;
				p.Y = 100;
				p.Z = 100;
				myTargetTrans = FTransform();
				myTargetTrans.SetLocation(p);
				
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
		myUseWeight = true;
		myTargetLocationWeight = 0.1f;
		myTargetRotationWeight = 0.8f;
		myTargetTrans.SetLocation(FVector(30, 30, 0));
		myTargetTrans.SetRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));
		break;
		
	case EFPCombatState::READY:
		
		break;
		
	case EFPCombatState::STRIKE:
		
		break;
		
	case EFPCombatState::DEFLECT:
		
		break;
	}
}

void UFPCombat::UpdateTransforms(float aDT)
{
	const auto character = GetFPCharacter();
	if (!character)
		return;
	
	const auto right = character->GetRightHand();
	const auto left = character->GetLeftHand();
	if (!right || !left)
		return;

	const auto animator = character->GetAnimator();
	if (!animator)
		return;

	const float locationWeight = FMath::Max(myTargetLocationWeight, animator->GetSwordPart()) * myUseWeight;
	const float rotationWeight = FMath::Max(myTargetRotationWeight, animator->GetSwordPart()) * myUseWeight;

	myLocationWeight = FMath::FInterpTo(myLocationWeight, locationWeight, aDT, mySmoothing);
	myRotationWeight = FMath::FInterpTo(myRotationWeight, rotationWeight, aDT, mySmoothing);

	const auto rTrans =
		LerpTrans(animator->GetRight(), myTrans, myLocationWeight, myRotationWeight);
	
	const auto lTrans =
		myUseBothHands ?
			LerpTrans(animator->GetLeft(), myTrans, myLocationWeight, myRotationWeight) :
			animator->GetLeft();
		
	DTLerpTrans(rTrans, myTargetTrans, aDT, mySmoothing);
	
	right->SetActorRelativeTransform(rTrans);
	left->SetActorRelativeTransform(lTrans);
	
	if(mySword && myState != EFPCombatState::NO_SWORD)
		mySword->SetActorRelativeTransform(rTrans);
}

ASword* UFPCombat::GetSword() const
{
	return mySword;
}

AFPCharacter* UFPCombat::GetFPCharacter() const
{
	return Cast<AFPCharacter>(GetOwner());
}

FTransform UFPCombat::LerpTrans(const FTransform& aFirst, const FTransform& aSecond, float aLocationWeight, float aRotationWeight)
{
	const auto location = FMath::Lerp(aFirst.GetLocation(), aSecond.GetLocation(), aLocationWeight);
	const auto rotation = FQuat::Slerp(aFirst.GetRotation(), aSecond.GetRotation(), aRotationWeight);
	return FTransform(rotation, location);
}

FTransform UFPCombat::DTLerpTrans(const FTransform& aFirst, const FTransform& aSecond, float aDT, float aSmoothing)
{
	const auto location = FMath::VInterpTo(aFirst.GetLocation(), aSecond.GetLocation(), aDT, aSmoothing);
	const auto rotation = FMath::QInterpTo(aFirst.GetRotation(), aSecond.GetRotation(), aDT, aSmoothing);
	return FTransform(rotation, location);
}
