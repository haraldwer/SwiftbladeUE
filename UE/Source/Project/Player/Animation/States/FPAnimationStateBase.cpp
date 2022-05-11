#include "FPAnimationStateBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Utility/Tools/CustomCamera.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"

UClass* UFPAnimationStateBase::Update(float aDT)
{
	const auto result = Super::Update(aDT);
	myTime += aDT; 
	return result; 
}

void UFPAnimationStateBase::SetHands(const FFPAnimationHandPositions& someHandPositions, const bool aSnap) const
{
	GetAnimator().SetHands(someHandPositions, aSnap);
}

FFPAnimationHandPositions UFPAnimationStateBase::GetTargetHands() const
{
	return GetAnimator().GetTargetHands();
}

FFPAnimationHandPositions UFPAnimationStateBase::GetRealHands() const
{
	return GetAnimator().GetRealHands();
}

void UFPAnimationStateBase::SetCamera(const FFPAnimationCameraData& someCameraData, bool aSnap) const
{
	GetAnimator().SetCamera(someCameraData, aSnap); 
}

FFPAnimationCameraData UFPAnimationStateBase::GetTargetCamera() const
{
	return GetAnimator().GetTargetCamera();
}

FFPAnimationCameraData UFPAnimationStateBase::GetRealCamera() const
{
	return GetAnimator().GetRealCamera();
}

FTransform UFPAnimationStateBase::FlipRightToLeft(const FTransform& aRightTransform)
{
	return aRightTransform *
		FTransform(
			FQuat::Identity,
			FVector::ZeroVector,
			FVector(1.0f, -1.0f, 1.0f));
}

FTransform UFPAnimationStateBase::ToCameraSpace(const FTransform& aTransform) const
{
	const auto camTrans = GetCamera().GetRelativeTransform();
	const auto adjustedTransform = FTransform(
		aTransform.GetRotation(),
		aTransform.GetLocation() - camTrans.GetLocation(),
		aTransform.GetScale3D());
	return adjustedTransform * camTrans; 
}

FVector UFPAnimationStateBase::Get3DNoise(const float aSpeed, const float aStrength, const float anOffset) const
{
	return FVector(
		(FMath::PerlinNoise1D((GetStateTime() + 100.0f + anOffset) * aSpeed) * 2.0f - 1.0f) * aStrength,
		(FMath::PerlinNoise1D((GetStateTime() + 200.0f + anOffset) * aSpeed) * 2.0f - 1.0f) * aStrength,
		(FMath::PerlinNoise1D((GetStateTime() + 300.0f + anOffset) * aSpeed) * 2.0f - 1.0f) * aStrength);
}

FFPAnimationHandCollision UFPAnimationStateBase::GetHandCollision(const FTransform& aHandTrans, const float aRadius) const
{
	FFPAnimationHandCollision result;

	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "Missing world", result);
	const auto& character = GetCharacter();
	const auto& combat = GetCombat();
	const auto charTrans = character.GetTransform();
	
	TArray<FHitResult> hits;
	FCollisionQueryParams params;
	params.bFindInitialOverlaps = true;
	params.AddIgnoredActor(Cast<AActor>(character.GetLeftHand()));
	params.AddIgnoredActor(Cast<AActor>(character.GetRightHand()));
	params.AddIgnoredActor(&character);
	if (const auto sword = combat.GetSword())
		params.AddIgnoredActor(Cast<AActor>(sword));

	const FVector localLocation = aHandTrans.GetLocation();
	const FVector location = charTrans.TransformPosition(localLocation);
	
	if (world->SweepMultiByChannel(
		hits,
		location,
		location + FVector(0.0f, 0.0f, 1.0f),
		aHandTrans.GetRotation(),
		ECC_Camera,
		FCollisionShape::MakeSphere(aRadius),
		params))
	{
		// Distance sort
		hits.Sort([&](const FHitResult& aFirst, const FHitResult& aSecond)
		{
			return FVector::DistSquared(aFirst.Location, location) < FVector::DistSquared(aSecond.Location, location);
		});

		for (const auto& hit : hits)
		{
			if (!hit.bBlockingHit)
				continue;; 
			
			result.myHit = true;
			result.myHitActor = hit.GetActor();

			// Palm towards collision 
			result.myTransform.SetLocation(charTrans.InverseTransformPosition(hit.ImpactPoint));
			auto rot = charTrans.InverseTransformRotation((hit.ImpactNormal * -1.0f).Rotation().Quaternion());
			auto adjusted = (rot * FRotator(90.0f, 0.0f, -90.0f).Quaternion()).Rotator();
			FQuat corrected = FMath::Lerp(adjusted.Quaternion(), FRotator(adjusted.Pitch, 0.0f, adjusted.Roll).Quaternion(), hit.ImpactNormal.Z);
			result.myTransform.SetRotation(corrected);
			
			return result; 
		}
	}

	return result;
}

void UFPAnimationStateBase::OverrideSwordData(FFPAnimationHandPositions& someData, const float aLocationWeight, float aRotationWeight, bool aDualWeild) const
{
	if (GetCombat().HasSword())
	{
		const auto swordTrans = GetAnimator().GetDefaultSwordTransform();
		const auto cameraTrans = LerpTransWeight(
			swordTrans,
			ToCameraSpace(swordTrans),
			0.7f, 0.7f);
		
		someData.myRight = LerpTransWeight(
			someData.myRight,
			cameraTrans,
			aLocationWeight,
			aRotationWeight);
		someData.myRightHandState = EHandState::CLOSED;
		if (aDualWeild)
		{
			someData.myLeft = FTransform(
				someData.myRight.GetRotation(),
				someData.myRight.TransformPosition(FVector(0, 0, 5)));
			someData.myLeftHandState = EHandState::CLOSED;
		}
	}
}

void UFPAnimationStateBase::OverrideVelocityData(FFPAnimationHandPositions& someData, const float aVelocityWeight, const float aDT) const
{
	const auto velocity = GetCharacterMovement().GetLastUpdateVelocity();
	const auto transformedVelocity = GetActorTransform().InverseTransformVectorNoScale(velocity);
	someData.myRight.SetLocation(someData.myRight.GetLocation() - transformedVelocity * aVelocityWeight * aDT);
	someData.myLeft.SetLocation(someData.myLeft.GetLocation() - transformedVelocity * aVelocityWeight * aDT);
}

FTransform UFPAnimationStateBase::GetDefaultHandTransform() const
{
	return GetRelativeTransform();
}
