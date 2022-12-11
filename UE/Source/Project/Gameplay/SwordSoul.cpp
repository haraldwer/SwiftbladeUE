#include "SwordSoul.h"

#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/Level/LevelManager.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/MainSingelton.h"

ASwordSoul::ASwordSoul()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASwordSoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto player = UMainSingelton::GetLocalPlayer();
	CHECK_RETURN(!player);
	
	const auto targetTrans = GetSplineTrans(player->GetActorLocation() + myDistanceOffset);
	const auto currentSplineTrans = GetSplineTrans(GetActorLocation());
	const FVector interpLoc = FMath::VInterpTo(currentSplineTrans.GetLocation(), targetTrans.GetLocation(), DeltaTime, mySmoothing);

	const float timeOffset = GetWorld()->GetTimeSeconds() * myOffsetNoiseTimeScale;
	const FVector offset =
		currentSplineTrans.InverseTransformVector(FVector::UpVector) * FMath::PerlinNoise1D(myDistance * myOffsetNoiseScale + timeOffset) * myOffsetNoiseStrength +
		currentSplineTrans.InverseTransformVector(FVector::RightVector) * FMath::PerlinNoise1D(myDistance * myOffsetNoiseScale + timeOffset + 1000.0f) * myOffsetNoiseStrength;

	const FVector loc = interpLoc + offset; 
	const FVector delta = loc - GetActorLocation();
	myDistance += delta.Size();
	SetActorLocation(interpLoc);

	LOG("Distance: " + FString::SanitizeFloat(myDistance));

	const FVector forwardLoc = currentSplineTrans.InverseTransformVector(FVector::ForwardVector * 100.0f);	
	const FRotator rot = UKismetMathLibrary::FindLookAtRotation(loc, forwardLoc);
	SetActorRotation(rot);
	
}

FTransform ASwordSoul::GetSplineTrans(const FVector aLocation, const float aDistanceOffset)
{
	const auto& levelMan = UMainSingelton::GetLevelManager();
	const auto spline = levelMan.GetPathSpline();
	CHECK_RETURN(!spline, FTransform::Identity);
	const float locKey = spline->FindInputKeyClosestToWorldLocation(aLocation);
	const float dist = spline->GetDistanceAlongSplineAtSplineInputKey(locKey) + aDistanceOffset;
	return spline->GetTransformAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
}
