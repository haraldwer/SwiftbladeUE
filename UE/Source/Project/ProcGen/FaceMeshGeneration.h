#pragma once

#include "Project/LevelGen/GeneratorBase.h"
#include "ProceduralMeshComponent.h"

inline void CreateFaceMesh(UProceduralMeshComponent* aComponent, const TArray<FVector2D>& someVertices, float aHeight, UMaterialInterface* aMaterial)
{
	CHECK_RETURN_LOG(!aComponent, "No proc mesh comp");
	CHECK_RETURN_LOG(someVertices.Num() < 3, "Can't generate mesh from less than three vertices");

	const int32 numVertices = someVertices.Num();
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FVector> normals;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	
	uvs.Reserve(numVertices * 4);
	normals.Reserve(numVertices * 4);
	vertexColors.Reserve(numVertices * 4);
	tangents.Reserve(numVertices * 4);

	FVector2D middle;
	for (auto& vert : someVertices)
		middle += vert;
	middle /= someVertices.Num();

	// Add top face
	vertices.Add(FVector(middle.X, middle.Y, 0));
	uvs.Add(FVector2D(0, 0));
	normals.Add(FVector::UpVector);
	vertexColors.Add(FLinearColor(1, 1, 1));
	tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	for (auto& flatVert : someVertices)
	{
		vertices.Add(FVector(flatVert.X, flatVert.Y, 0));
		uvs.Add(FVector2D(0, 0));
		normals.Add(FVector::UpVector);
		vertexColors.Add(FLinearColor(1, 1, 1));
		tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}
	for (int32 i = 0; i < numVertices + 1; i++)
	{
		triangles.Add(0);
		triangles.Add(1 + (i + 1) % numVertices);
		triangles.Add(1 + i % numVertices);
	}

	// Add bottom face
	vertices.Add(FVector(middle.X, middle.Y, -aHeight));
	uvs.Add(FVector2D(0, 0));
	normals.Add(FVector::DownVector);
	vertexColors.Add(FLinearColor(1, 1, 1));
	tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	for (auto& flatVert : someVertices)
	{
		vertices.Add(FVector(flatVert.X, flatVert.Y, -aHeight));
		uvs.Add(FVector2D(0, 0));
		normals.Add(FVector::DownVector);
		vertexColors.Add(FLinearColor(1, 1, 1));
		tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}
	for (int32 i = 0; i < numVertices + 1; i++)
	{
		triangles.Add(numVertices + 2 + (i % numVertices));
		triangles.Add(numVertices + 2 + (i + 1) % numVertices);
		triangles.Add(numVertices + 2);
	}

	const TArray<FVector> prevVertices = vertices;
	
	// Add sides
	for (int32 i = 0; i < numVertices; i++)
	{		
		const int32 currI = i;
		const int32 nextI = (i + 1) % numVertices;
		
		// Add vertex duplicates (for improved normals)
		
		// Top
		vertices.Add(prevVertices[currI + 1]);
		vertices.Add(prevVertices[nextI + 1]);
		// Bottom
		vertices.Add(prevVertices[currI + numVertices + 2]);
		vertices.Add(prevVertices[nextI + numVertices + 2]);
		
		const int32 indexOffset = numVertices * 2 + 2;
		const int32 currIndex = indexOffset + currI * 4;
		const int32 nextIndex = indexOffset + nextI * 4;
		
		const FVector2D first = someVertices[currI];
		const FVector2D second = someVertices[nextI];
		const FVector2D diff = (second - first).GetSafeNormal();
		FVector2D flatNormal = -FVector2D(-diff.Y, diff.X);
		if (true)//flatNormal.Dot(aFace.location - (first + second) * 0.5f) < 0.0f)
		{
			triangles.Add(currIndex);
			triangles.Add(nextIndex);
			triangles.Add(nextIndex + 2); // +2 for bottom vert
		
			triangles.Add(currIndex);
			triangles.Add(nextIndex + 2);
			triangles.Add(currIndex + 2);
		}
		else
		{
			// Reverse order
			triangles.Add(nextIndex + 2);
			triangles.Add(nextIndex);
			triangles.Add(currIndex);
		
			triangles.Add(currIndex + 2);
			triangles.Add(nextIndex + 2);
			triangles.Add(currIndex);

			flatNormal *= -1.0f; 
		}
		
		FVector normal = FVector(flatNormal.X, flatNormal.Y, 0.0f); 
		
		uvs.Add(FVector2D(0, 0));
		uvs.Add(FVector2D(0, 0));
		uvs.Add(FVector2D(0, 0));
		uvs.Add(FVector2D(0, 0));
		normals.Add(normal);
		normals.Add(normal);
		normals.Add(normal);
		normals.Add(normal);
		vertexColors.Add(FLinearColor(1, 1, 1));
		vertexColors.Add(FLinearColor(1, 1, 1));
		vertexColors.Add(FLinearColor(1, 1, 1));
		vertexColors.Add(FLinearColor(1, 1, 1));
		
		const FVector t1 = normal.Cross(FVector::ForwardVector);
		const FVector t2 = normal.Cross(FVector::UpVector);
		const FVector tangent = t1.SizeSquared() > t2.SizeSquared() ? t1 : t2;
		const FProcMeshTangent procTangent = FProcMeshTangent(tangent.X, tangent.Y, tangent.Z); 
		tangents.Add(procTangent);
		tangents.Add(procTangent);
		tangents.Add(procTangent);
		tangents.Add(procTangent);
	}

	aComponent->ClearAllMeshSections();
	aComponent->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	aComponent->SetMaterial(0, aMaterial);
}


inline UProceduralMeshComponent* CreateFaceMesh(AActor* anOwner, const TArray<FVector2D>& someVertices, const FVector& anOffset, float aHeight, UMaterialInterface* aMaterial)
{
	CHECK_RETURN_LOG(!anOwner, "No owner", nullptr);
	
	UProceduralMeshComponent* meshComp = NewObject<UProceduralMeshComponent>(anOwner);
	if (const auto generator = Cast<AGeneratorBase>(anOwner))
		generator->AddGeneratedObject(meshComp);
	meshComp->AttachToComponent(anOwner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	meshComp->SetWorldLocation(anOffset);
	meshComp->RegisterComponent();

	CreateFaceMesh(meshComp, someVertices, aHeight, aMaterial);
	return meshComp;
}
