#include "SwordSoul.h"

#include "Components/SplineComponent.h"
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
	
	const auto& levelMan = UMainSingelton::GetLevelManager();
	const auto spline = levelMan.GetPathSpline();
	CHECK_RETURN(!spline);
	
	const FVector loc = player->GetActorLocation();
	const float locKey = spline->FindInputKeyClosestToWorldLocation(loc);
	const float dist = spline->GetDistanceAlongSplineAtSplineInputKey(locKey) + myDistanceOffset;
	const FVector desiredLoc = spline->GetLocationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
	const FRotator desiredRot = spline->GetRotationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
	const FVector interpLoc = FMath::VInterpTo(GetActorLocation(), desiredLoc, DeltaTime, mySmoothing); 
	SetActorLocation(interpLoc);
	SetActorRotation(desiredRot);
}
