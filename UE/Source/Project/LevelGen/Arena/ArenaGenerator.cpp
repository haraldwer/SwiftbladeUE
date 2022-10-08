#include "ArenaGenerator.h"
#include "ArenaConfig.h"
#include "ArenaDataStructs.h"
#include "Components/ArenaCompBase.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/EngineUtility.h"
#include "Project/Utility/MainSingelton.h"

void AArenaGenerator::Generate()
{
	Super::Generate(); 

	// Select random config
	const UArenaConfig* config = GetRandomConfig();
	if (!config)
		return;

	TArray<FArenaLayer> layers;

	float prevLayerRadius = 0.0f;
	const int32 numSections = (ULevelRand::RandRange(config->myMinSections, config->myMaxSections) / 2) * 2; 
	
	// Create layers
	for (const auto& layerConfig : config->myLayerConfigs)
	{
		auto& layer = layers.Emplace_GetRef();
		layer.startRadius = prevLayerRadius;
		layer.endRadius = layer.startRadius + ULevelRand::FRandRange(layerConfig.myMinRadius, layerConfig.myMaxRadius);

		// Subdivisions are repeated for each section
		TArray<FArenaSubdivision> subdivisions;
		const int32 numSubdivisions = config->mySubdivisions;
		
		// Set up subdivisions
		for (int32 subIndex = 0; subIndex < numSubdivisions; subIndex++)
		{
			auto& subdivision = subdivisions.Emplace_GetRef();
			subdivision.height = ULevelRand::FRandRange(layerConfig.myMinSubHeight, layerConfig.myMaxSubHeight);
			
			// Start with getting components
			TArray<UArenaCompBase*> comps;
			for (auto& comp : layerConfig.myComponents)
				if (auto compPtr = comp.Get())
					if (ULevelRand::FRandRange(0.0f, 100.0f) < compPtr->GetChance())
						comps.Add(compPtr);
			
			subdivision.components = GetComponents(comps); 
		}
		
		// Create sections
		const float angleSize = PI * 2.0f / static_cast<float>(numSections * numSubdivisions);
		for (int32 sectionIndex = 0; sectionIndex < numSections; sectionIndex++)
		{
			auto& section = layer.sections.Emplace_GetRef();
			
			// Copy subdivisions for each section
			section.subdivisions = subdivisions;

			// Create subdivision mesh
			for (int32 subIndex = 0; subIndex < section.subdivisions.Num(); subIndex++)
			{
				auto& subdivision = section.subdivisions[subIndex];
				const float angle = angleSize * (sectionIndex * numSubdivisions + subIndex);
				subdivision.startAngle = angle - angleSize * 0.5f;
				subdivision.endAngle = angle + angleSize * 0.5f;

				const FVector2D startCoord = FVector2D(
				   FMath::Cos(subdivision.startAngle),
				   FMath::Sin(subdivision.startAngle));
				const FVector2D endCoord = FVector2D(
					FMath::Cos(subdivision.endAngle),
					FMath::Sin(subdivision.endAngle));
				
				subdivision.vertices.Add(endCoord * FMath::Max(0.1f, layer.startRadius));
				subdivision.vertices.Add(startCoord * FMath::Max(0.1f, layer.startRadius));
				subdivision.vertices.Add(startCoord * layer.endRadius);
				subdivision.vertices.Add(endCoord * layer.endRadius);
			}
		}

		prevLayerRadius = layer.endRadius;
	}

	// Populate
	for (const auto& layer : layers)
		for (const auto& section : layer.sections)
			for (const auto& subdivision : section.subdivisions)
				for (const auto& component : subdivision.components)
					if (component)
						component->Populate(this, layer, section, subdivision);

	CHECK_RETURN_LOG(!layers.Num(), "No layers");
	
	CreateCheckpoint(config, layers[0]);
	CreateDoor(config, layers.Last());
}

void AArenaGenerator::CreateCheckpoint(const UArenaConfig* aConfig, const FArenaLayer& aLayer)
{
	CHECK_RETURN(!aConfig)
	const FTransform trans = FTransform::Identity; 
	SpawnGeneratedActor(aConfig->myCheckpointClass.Get(), trans);
}

void AArenaGenerator::CreateDoor(const UArenaConfig* aConfig, const FArenaLayer& aLayer)
{
	CHECK_RETURN(!aConfig)
	const FVector location = FVector(0.0f, -aLayer.endRadius * aConfig->myDoorRadiusPart, aConfig->myDoorHeight);
	SpawnGeneratedActor(aConfig->myDoorClass, FTransform(location));

	if (UEngineUtility::IsInBaseLevel())
		if (const auto player = UMainSingelton::GetLocalPlayer())
			player->SetActorLocation(location - FVector::RightVector * 250.0f);
}

const UArenaConfig* AArenaGenerator::GetRandomConfig() const
{
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs", nullptr);
	const int32 index = ULevelRand::RandRange(0, myConfigs.Num() - 1);
	const auto configClass = myConfigs[index];
	const auto config = configClass.GetDefaultObject();
	CHECK_RETURN_LOG(!config, "No config for type", nullptr);
	return config;
}

