#include "SectionComponent.h"

#include "ChapterPreset.h"
#include "LevelRand.h"
#include "SectionPreset.h"

void USectionComponent::GenerateLevelOrder(const int32 aChapter, TArray<FString>& someLevels, TArray<int32>& someArenaIndices) const
{
	CHECK_RETURN_LOG(!myChapters.IsValidIndex(aChapter), "Invalid chapter index");
	const TSubclassOf<UChapterPreset>& chapterClass = myChapters[aChapter];
	UChapterPreset* chapterPtr = chapterClass.GetDefaultObject();
	CHECK_RETURN_LOG(!chapterPtr, "Invalid chapter ptr");
	UChapterPreset& chapter = *chapterPtr;	
	
	someLevels.Reset();
	someLevels.Add(chapter.myStart + "_0"); // Start of chapter
	for (int i = 0; i < chapter.mySections.Num(); i++)
	{
		TSubclassOf<USectionPreset>& sectionClass = chapter.mySections[i];
		USectionPreset* sectionPtr = sectionClass.GetDefaultObject();
		CHECK_CONTINUE_LOG(!sectionPtr, "Invalid section ptr")
		USectionPreset& section = *sectionPtr;

		// Ending number
		const FString str = "_" + FString::FromInt(i);

		// Section start
		if (i > 0)
			someLevels.Add(section.mySectionStart + str);

		// The actual rooms
		TArray<FSectionPresetRoom> pool = section.myRooms;
		const int32 numRooms = section.myNumRoomsToUse > 0 ? section.myNumRoomsToUse : pool.Num(); 
		for (int32 roomNum = 0; roomNum < numRooms; roomNum++)
		{
			CHECK_BREAK(!pool.Num())

			// Calculate total weight
			float totalWeight = 0.0f; 
			for (const FSectionPresetRoom& room : pool)
				totalWeight += room.myWeight;

			// Random
			float rand = ULevelRand::FRandRange(0.0f, totalWeight);
			for (int roomI = 0; roomI < pool.Num(); roomI++)
			{
				// Subtract until hit
				rand -= pool[roomI].myWeight;
				if (rand <= 0.0f)
				{
					// use this room!
					someLevels.Add(pool[roomI].myLevelName + str);
					pool.RemoveAtSwap(roomI);
					break; 
				}
			}
		}

		// Arena
		if (!section.myArena.IsEmpty())
		{
			someLevels.Add(section.mySectionEnd + str);
			someArenaIndices.Add(someLevels.Num());
			someLevels.Add(section.myArena + str);
		}
	}
	someLevels.Add(chapter.myEnd + "_0"); // End of chapter
}
