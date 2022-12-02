#include "SectionCompGroup.h"

#include "Project/LevelGen/LevelRand.h"

TArray<FGeneratorCompEntry<USectionCompBase>> USectionCompGroup::GetSubComponents(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) const
{
	TArray<FGeneratorCompEntry<USectionCompBase>> result; 
	TArray<TObjectPtr<USectionCompBase>> pool = myComponents;
	const int32 numComps = ULevelRand::RandRange(myMinNumComps, myMaxNumComps);
	for (int32 i = 0; i < numComps; i++)
	{
		float totalWeight = 0.0f;
		for (const auto& comp : pool)
			if (const auto ptr = comp.Get())
				totalWeight += ptr->GetChance();
		CHECK_BREAK(totalWeight <= 0.0f);		
		
		float randWeight = ULevelRand::FRandRange(0.0f, totalWeight);
		for (int32 j = 0; j < pool.Num(); j++)
		{
			auto ptr = pool[j].Get();
			CHECK_CONTINUE(!ptr); 
			randWeight -= ptr->GetChance();
			CHECK_CONTINUE(randWeight >= 0.0f)
			auto& entry = result.Emplace_GetRef();
			entry.myPtr = ptr;
			entry.myRequiredComps = ptr->GetReqiredComps();
			entry.myRequiredComps.Append(GetReqiredComps());
			entry.myBlockingComps = ptr->GetBlockingComps();
			entry.myBlockingComps.Append(GetBlockingComps());
			if (!myReuse)
				pool.RemoveAtSwap(j); 
			break;
		}
	}
	return result; 
}