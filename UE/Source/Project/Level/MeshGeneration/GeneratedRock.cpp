#include "GeneratedRock.h"
#include "ProceduralMeshComponent.h" 
#include "RockGeneration.h"

AGeneratedRock::AGeneratedRock()
{
	PrimaryActorTick.bCanEverTick = false;

	myCustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh");
	CHECK_ASSERT(!myCustomMesh, "Failed to create custom mesh component");
	SetRootComponent(myCustomMesh);
	myCustomMesh->bUseAsyncCooking = true;
	
	// GenerateMesh();
}

void AGeneratedRock::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGeneratedRock::GenerateMesh() const
{
	const auto mesh = MakeIcosphere(mySubdivisions);
	
	myCustomMesh->CreateMeshSection(0,
		mesh.myVertices,
		mesh.myIndices,
		TArray<FVector>(),
		TArray<FVector2D>(),
		TArray<FColor>(),
		TArray<FProcMeshTangent>(),
		true);
}
