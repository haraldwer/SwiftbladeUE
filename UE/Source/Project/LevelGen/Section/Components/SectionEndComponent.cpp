#include "SectionEndComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project/LevelGen/Level/LevelEndLocation.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionEndComponent::Populate(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	if (!aSection.rooms.Num())
		return;

	auto& lastRoom = aSection.rooms.Last();
	
	TArray<AActor*> levelEnds;
	UGameplayStatics::GetAllActorsOfClass(aGenerator->GetWorld(), ALevelEndLocation::StaticClass(), levelEnds);
	for (AActor* actor : levelEnds)
	{
		if (const auto levelEnd = Cast<ALevelEndLocation>(actor))
		{
			levelEnd->SetActorLocation(FVector(aSection.lastEdgeLoc.X, aSection.lastEdgeLoc.Y, lastRoom.groundOffset + lastRoom.ceilHeight * 0.5f));
			levelEnd->OnEndLocationSet(); 
		}
	}
}
