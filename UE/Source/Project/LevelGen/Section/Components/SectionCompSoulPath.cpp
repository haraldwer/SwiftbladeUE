#include "SectionCompSoulPath.h"

#include "Components/SplineComponent.h"
#include "Project/LevelGen/Level/LevelManager.h"
#include "Project/Utility/MainSingelton.h"

void USectionCompSoulPath::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	if (!IsValid(GEngine->GetCurrentPlayWorld()))
		return; 
	
	if (const auto pathSpline = UMainSingelton::GetLevelManager().GetPathSpline())
	{
		const float part = 1.0f / static_cast<float>(myPathPrecision + 1);
		for (int32 i = 0; i < myPathPrecision; i++)
		{
			const float progress = ((i * part) * (1.0f - myPathPadding * 2.0f)) + myPathPadding;				
			const float index = progress * aRoom.path.Num();
			const FVector2D pathLoc = GetBlendVert(aRoom.path, index);
			const FVector location = FVector(pathLoc.X, pathLoc.Y, aRoom.groundOffset + myGroundOffset);
			pathSpline->AddSplinePoint(location, ESplineCoordinateSpace::World, false);
		}
	}
}
