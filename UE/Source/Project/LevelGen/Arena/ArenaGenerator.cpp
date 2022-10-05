#include "ArenaGenerator.h"
#include "ArenaConfig.h"
#include "ArenaDataStructs.h"
#include "Components/ArenaCompBase.h"
#include "Project/Gameplay/Checkpoint.h"

void AArenaGenerator::Generate()
{
	Super::Generate(); 

	// Select random config
	const UArenaConfig* config = GetRandomConfig();
	if (!config)
		return;

	TArray<FArenaLayer> layers;

	// Create layers
	for (const auto& layerConfig : config->myLayerConfigs)
	{
		const auto* prevLayer = layers.Num() > 0 ? &layers.Last() : nullptr; 
		auto& layer = layers.Emplace_GetRef();
		layer.startRadius = prevLayer ? prevLayer->endRadius : 0.0f;
		layer.endRadius = layer.startRadius + FMath::RandRange(layerConfig.myMinRadius, layerConfig.myMaxRadius);

		// Subdivisions are repeated for each section
		// Start with getting components
		TArray<FArenaSubdivision> subdivisions;
		const int32 numSubdivisions = layerConfig.mySubdivisions;
		for (int32 subIndex = 0; subIndex < numSubdivisions; subIndex++)
		{
			auto& subdivision = subdivisions.Emplace_GetRef();
			subdivision.height = FMath::RandRange(layerConfig.myMinSubHeight, layerConfig.myMaxSubHeight); 
			GenerateSubComponents(layerConfig, layer, subdivision);
		}
		
		// Create sections
		const int32 prevNumSections = prevLayer ? prevLayer->sections.Num() : 1;
		const int32 randNumSections = FMath::RandRange(config->myLayerMinSections, config->myLayerMaxSections);
		const int32 roundNumSections = (randNumSections / prevNumSections) * prevNumSections;
		const int32 numSections = FMath::Max(prevNumSections, roundNumSections);
		const float angleSize = PI * 2.0f / static_cast<float>(numSections * numSubdivisions);
		for (int32 sectionIndex = 0; sectionIndex < numSections; sectionIndex++)
		{
			auto& section = layer.sections.Emplace_GetRef();
			section.subdivisions = subdivisions;	
			for (int32 subIndex = 0; subIndex < section.subdivisions.Num(); subIndex++)
			{
				auto& subdivision = section.subdivisions[subIndex];
				const float angle = angleSize * (sectionIndex * numSubdivisions + subIndex);
				const float startAngle = angle - angleSize * 0.5f;
				const float endAngle = angle + angleSize * 0.5f;
				CreateSubMesh(
					FMath::Max(0.1f, layer.startRadius),
					layer.endRadius,
					startAngle, endAngle,
					subdivision);
			}
		}
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


void AArenaGenerator::GenerateSubComponents(const FArenaConfigLayer& aConfig, const FArenaLayer& aLayer, FArenaSubdivision& aSubdivision) const
{
	CHECK_RETURN_LOG(!aConfig.myComponents.Num(), "No components specified for layer");
	
	TArray<TObjectPtr<UArenaCompBase>> comps = aConfig.myComponents;
	while (aSubdivision.components.Num() < aConfig.myNumSubdivisionComponents && comps.Num())
	{
		const int32 compIndex = FMath::RandRange(0, comps.Num() - 1);
		aSubdivision.components.Add(comps[compIndex]);
		comps.RemoveAtSwap(compIndex);
	}
}

void AArenaGenerator::CreateSubMesh(const float aStartRadius, const float anEndRadius, const float aStartAngle, const float anEndAngle, FArenaSubdivision& aSubdivision) const
{
	const FVector2D startCoord = FVector2D(
		FMath::Cos(aStartAngle),
		FMath::Sin(aStartAngle));
	const FVector2D endCoord = FVector2D(
		FMath::Cos(anEndAngle),
		FMath::Sin(anEndAngle));
	
	aSubdivision.vertices.Add(startCoord * aStartRadius);
	aSubdivision.vertices.Add(startCoord * anEndRadius);			
	aSubdivision.vertices.Add(endCoord * anEndRadius);
	aSubdivision.vertices.Add(endCoord * aStartRadius);
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
	const FTransform trans = FTransform(FVector(0.0f, -aLayer.endRadius, 0.0f)); 
	SpawnGeneratedActor(aConfig->myDoorClass, trans);
}

const UArenaConfig* AArenaGenerator::GetRandomConfig() const
{
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs", nullptr);
	const int32 index = FMath::RandRange(0, myConfigs.Num() - 1);
	const auto configClass = myConfigs[index];
	const auto config = configClass.GetDefaultObject();
	CHECK_RETURN_LOG(!config, "No config for type", nullptr);
	return config;
}

