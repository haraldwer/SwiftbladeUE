#include "SectionCompCorner.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompCorner::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection,const FProcRoom& aRoom)
{
	for (auto& vert : aRoom.vertices)
	{
		const FVector loc = FVector(vert.X, vert.Y, aRoom.groundOffset + aRoom.ceilHeight * myCeilPart);
		const FVector2D dir = -(vert - aRoom.center).GetSafeNormal();
		const float rot = FMath::RadiansToDegrees(FMath::Atan2(dir.Y, dir.X));

		if (const auto meshActor = Cast<AStaticMeshActor>(aGenerator->SpawnGeneratedActor(AStaticMeshActor::StaticClass(), FTransform(FRotator(0.0f, rot, 0.0f), loc))))
		{
			const auto bounds = myMesh->GetBounds();
			const float height = bounds.BoxExtent.Z * 2.0f;
			meshActor->SetActorScale3D(FVector(1.0f, 1.0f, (1.0f / height) * aRoom.ceilHeight));
			if (const auto meshComp = meshActor->GetStaticMeshComponent())
			{
				meshActor->SetMobility(EComponentMobility::Movable);
				meshComp->SetMobility(EComponentMobility::Movable);
				meshComp->SetStaticMesh(myMesh);
				meshActor->SetMobility(EComponentMobility::Static);
				meshComp->SetMobility(EComponentMobility::Static);
			}
		}
	}
}
