#pragma once

#include "LevelData.h"
#include "ProceduralMeshComponent.h"

inline UProceduralMeshComponent* CreateFaceMesh(ALevelData* aLevelData, const FLevelDataFace& aFace, const FVector& anOffset, float aHeight, UMaterialInterface* aMaterial)
{
	CHECK_RETURN_LOG(!aLevelData, "No level data", nullptr);
	CHECK_RETURN_LOG(aFace.vertices.Num() < 3, "Can't generate mesh from less than three vertices", nullptr);
	
	UProceduralMeshComponent* meshComp = NewObject<UProceduralMeshComponent>(aLevelData);
	aLevelData->AddGeneratedObject(meshComp);
	meshComp->AttachToComponent(aLevelData->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	meshComp->SetRelativeLocation(anOffset);
	meshComp->RegisterComponent();

	const int32 numVertices = aFace.vertices.Num();
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

	// Add top face
	for (auto& flatVert : aFace.vertices)
	{
		vertices.Add(FVector(flatVert.X, flatVert.Y, 0));
		uvs.Add(FVector2D(0, 0));
		normals.Add(FVector::UpVector);
		vertexColors.Add(FLinearColor(1, 1, 1));
		tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}
	for (int32 i = numVertices - 2; i >= 1; i--)
	{
		triangles.Add(numVertices - 1);
		triangles.Add(i);
		triangles.Add(i - 1);
	}

	// Add bottom face
	for (auto& flatVert : aFace.vertices)
	{
		vertices.Add(FVector(flatVert.X, flatVert.Y, -aHeight));
		uvs.Add(FVector2D(0, 0));
		normals.Add(FVector::DownVector);
		vertexColors.Add(FLinearColor(1, 1, 1));
		tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}
	for (int32 i = numVertices + 1; i < numVertices * 2 - 1; i++)
	{
		triangles.Add(numVertices);
		triangles.Add(i);
		triangles.Add(i + 1);
	}

	const TArray<FVector> prevVertices = vertices;
	
	// Add sides
	for (int32 i = 0; i < numVertices; i++)
	{		
		const int32 indexOffset = numVertices * 2;
		const int32 currI = i;
		const int32 nextI = (i + 1) % numVertices;
		
		// Add vertex duplicates (for improved normals)
		
		// Top
		vertices.Add(prevVertices[currI]);
		vertices.Add(prevVertices[nextI]);
		// Bottom
		vertices.Add(prevVertices[currI + numVertices]);
		vertices.Add(prevVertices[nextI + numVertices]);
		
		const int32 currIndex = indexOffset + currI * 4;
		const int32 nextIndex = indexOffset + nextI * 4;
		
		const FVector2D first = aFace.vertices[currI];
		const FVector2D second = aFace.vertices[nextI];
		const FVector2D diff = (second - first).GetSafeNormal();
		FVector2D flatNormal = -FVector2D(-diff.Y, diff.X);
		if (flatNormal.Dot(aFace.location - (first + second) * 0.5f) < 0.0f)
		{
			triangles.Add(currIndex);
			triangles.Add(nextIndex);
			triangles.Add(nextIndex + 2);
		
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

	meshComp->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	meshComp->SetMaterial(0, aMaterial);
	return meshComp;
}
