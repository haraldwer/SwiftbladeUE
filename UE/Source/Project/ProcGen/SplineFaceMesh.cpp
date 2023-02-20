
#include "SplineFaceMesh.h"

#include "FaceMeshGeneration.h"
#include "Components/SplineComponent.h"

ASplineFaceMesh::ASplineFaceMesh()
{
	PrimaryActorTick.bCanEverTick = false;

	auto root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);
	root->SetMobility(EComponentMobility::Static);
	
	mySpline = CreateDefaultSubobject<USplineComponent>("Spline");
	mySpline->SetupAttachment(RootComponent);
	mySpline->SetMobility(EComponentMobility::Static);

	myMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	myMesh->SetupAttachment(RootComponent);

	Generate();
}

void ASplineFaceMesh::BeginPlay()
{
	Super::BeginPlay();
	Generate();
}

void ASplineFaceMesh::Generate() const
{
	if (const auto spline = mySpline.Get())
	{
		float dist = 0.0f;
		TArray<FVector2D> verts; 
		while (dist < spline->GetSplineLength())
		{
			const FVector loc = spline->GetLocationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::Local);
			verts.Add({loc.X, loc.Y});
			dist += myPrecision; 
		}
		if (const auto mesh = myMesh.Get())
		{
			mesh->SetMobility(EComponentMobility::Movable);
			mesh->SetWorldLocation(GetActorLocation());
			mesh->SetMobility(EComponentMobility::Static);
		}
		CreateFaceMesh(myMesh.Get(), verts, myHeight, myMaterial);
	}
}
