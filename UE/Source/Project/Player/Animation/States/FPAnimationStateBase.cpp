#include "FPAnimationStateBase.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Interaction.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Utility/Tools/CustomCamera.h"

void UFPAnimationStateBase::Enter()
{
	Super::Enter();
	myTime = 0.0f;
}

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

FTransform UFPAnimationStateBase::DualWeild(const FTransform& aRightTransform)
{
	return FTransform(aRightTransform.GetRotation(), aRightTransform.TransformPosition(FVector(0, 0, 5)));
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

EHandState UFPAnimationStateBase::GetSwordHandState() const
{
	return GetCombat().GetHasSword() ? EHandState::CLOSED : EHandState::OPEN; 
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

			result.myTransform.SetLocation(charTrans.InverseTransformPosition(hit.ImpactPoint));
			auto rot = charTrans.InverseTransformRotation((hit.ImpactNormal * -1.0f).Rotation().Quaternion());
			auto adjusted = (rot * FRotator(90.0f, 0.0f, -90.0f).Quaternion()).Rotator();
			
			// Rotation is not correct when normal is from straight up
			// Override yaw when looking from above
			// Make exponential?
			FQuat corrected = FMath::Lerp(adjusted.Quaternion(), FRotator(adjusted.Pitch, 0.0f, adjusted.Roll).Quaternion(), hit.ImpactNormal.Z);
			result.myTransform.SetRotation(corrected);
			
			return result; 
		}
	}

	return result;
}

FTransform UFPAnimationStateBase::GetDefaultHandTransform() const
{
	return GetRelativeTransform();
}
