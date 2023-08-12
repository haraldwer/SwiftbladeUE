#include "SwordSoul.h"

#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "..\LevelGen\LevelSubsystem.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/GameUtility.h"

ASwordSoul::ASwordSoul()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASwordSoul::BeginPlay()
{
	Super::BeginPlay();

	myStartLocation = GetActorLocation();
	myStartTimestamp = GetWorld()->GetTimeSeconds();
}

void ASwordSoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const auto player = UGameUtility::GetLocalPlayer();
	CHECK_RETURN(!player);

	const auto& levelMan = ULevelSubsystem::Get(); 
	const auto spline = levelMan.GetPathSpline();
	CHECK_RETURN(!spline);
	const float locKey = spline->FindInputKeyClosestToWorldLocation(player->GetActorLocation());
	const float desiredDist = spline->GetDistanceAlongSplineAtSplineInputKey(locKey) + myDistanceOffset;
	//DrawDebugSphere(GetWorld(), spline->GetLocationAtSplineInputKey(locKey, ESplineCoordinateSpace::World), 10.0f, 8, FColor::Green);
	
	myDistance = FMath::FInterpTo(myDistance, desiredDist, DeltaTime, mySmoothing);
	auto trans = spline->GetTransformAtDistanceAlongSpline(myDistance, ESplineCoordinateSpace::World);
	trans.SetLocation(trans.GetLocation() + FVector::UpVector * myVerticalOffset);
	//DrawDebugSphere(GetWorld(), trans.GetLocation(), 10.0f, 8, FColor::Magenta);
	
	const float timeOffset = GetWorld()->GetTimeSeconds() * myOffsetNoiseTimeScale;
	const FVector offset = trans.TransformVector(FVector(
			FMath::PerlinNoise1D(myDistance * myOffsetNoiseScale + timeOffset) * myOffsetNoiseStrength,
			FMath::PerlinNoise1D(myDistance * myOffsetNoiseScale + timeOffset + 1000.0f) * myOffsetNoiseStrength,
			FMath::PerlinNoise1D(myDistance * myOffsetNoiseScale + timeOffset + 2000.0f) * myOffsetNoiseStrength
		) * myOffsetMul);

	const float time = GetWorld()->GetTimeSeconds() - myStartTimestamp; 
	const float startCurveVal = myStartCurve.EditorCurveData.Eval(time); 
	const FVector loc = FMath::Lerp(myStartLocation, trans.GetLocation() + offset, startCurveVal);
	
	const FVector forwardLook = trans.GetRotation().Vector();
	const FVector headingLook = (loc - GetActorLocation()).GetSafeNormal();
	const FVector look = forwardLook + headingLook;
	const FRotator rot = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, look);

	const FRotator smoothRot = FMath::RInterpTo(GetActorRotation(), rot, DeltaTime, myRotSmoothing); 
	SetActorRotation(smoothRot);

	SetActorLocation(loc);
}

FTransform ASwordSoul::GetSplineTrans(const FVector aLocation, const float aDistanceOffset)
{
	const auto& levelMan = ULevelSubsystem::Get(); 
	const auto spline = levelMan.GetPathSpline();
	CHECK_RETURN(!spline, FTransform::Identity);
	const float locKey = spline->FindInputKeyClosestToWorldLocation(aLocation);
	const float dist = spline->GetDistanceAlongSplineAtSplineInputKey(locKey) + aDistanceOffset;
	return spline->GetTransformAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
}
