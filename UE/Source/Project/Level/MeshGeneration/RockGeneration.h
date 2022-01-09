#pragma once
#include "CoreMinimal.h"

using Index=int32;

struct Triangle
{
    Index myVertex[3];
};
 
using TriangleList=TArray<Triangle>;
using VertexList=TArray<FVector>;
using IndexList=TArray<Index>;
 
namespace Icosahedron
{
    constexpr float X=.525731112119133606f;
    constexpr float Z=.850650808352039932f;
    constexpr float N=0.f;
 
    static const VertexList vertices=
    {
        {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
        {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
        {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
    };
 
    static const TriangleList triangles=
    {
        {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
        {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
        {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
        {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
    };
}

using Lookup=TMap<TPair<Index, Index>, Index>;
 
Index VertexForEdge(Lookup& aLookup, VertexList& someVertices, Index aFirst, Index aSecond)
{
    Lookup::KeyType key(aFirst, aSecond);
    if (key.Key > key.Value)
    {
        const auto tmpKey = key.Key;
        key.Key = key.Value;
        key.Value = tmpKey;
    }

    auto& inserted = aLookup.FindOrAdd(key);
    inserted = someVertices.Num();
    if (inserted)
    {
        const auto& edge0=someVertices[aFirst];
        const auto& edge1=someVertices[aSecond];
        const auto point= (edge0 + edge1).GetSafeNormal();
        someVertices.Add(point);
    }
 
    return key.Value;
}

inline TriangleList Subdivide(VertexList& someVertices, TriangleList someTriangles)
{
    Lookup lookup;
    TriangleList result;
 
    for (auto&& each:someTriangles)
    {
        Index mid[3];
        for (int edge=0; edge<3; ++edge)
        {
            mid[edge]=VertexForEdge(lookup, someVertices,
              each.myVertex[edge], each.myVertex[(edge+1)%3]);
        }
 
        result.Add({each.myVertex[0], mid[0], mid[2]});
        result.Add({each.myVertex[1], mid[1], mid[0]});
        result.Add({each.myVertex[2], mid[2], mid[1]});
        result.Add({mid[0], mid[1], mid[2]});
    }
 
    return result;
}

struct Mesh
{
    VertexList myVertices;
    IndexList myIndices;
};

inline Mesh MakeIcosphere(const int aSubdivisions)
{
    VertexList vertices=Icosahedron::vertices;
    TriangleList triangles=Icosahedron::triangles;
 
    for (int i=0; i<aSubdivisions; ++i)
    {
        triangles=Subdivide(vertices, triangles);
    }

    IndexList indices;
    for (const auto& it : triangles)
        indices.Append(it.myVertex, 3);
    return { vertices, indices };
}






/*
struct Triangle
{
    int32 p1;
    int32 p2;
    int32 p3;
};

struct SphereMesh
{
    TArray<FVector> myVertices;
    TArray<int32> myIndices;
};

inline void ProjectToUnitSphere(SphereMesh& aMesh)
{
    for (auto& v : aMesh.myVertices)
        v.Normalize();
}

inline Loop(SphereMesh& mesh)
{
    // For each triangle?
    for (int i = 0; i < mesh.myIndices.Num(); i += 3)
    {
        // New point on every edge
        
        // Connect new points

        
    }
}

inline SphereMesh Icosahedron()
{
    SphereMesh mesh;

    const float phi = (1.0f + sqrtf(5.0f)) * 0.5f; // golden ratio
    const float a = 1.0f;
    const float b = 1.0f / phi;

    // add vertices
    const auto v1 = mesh.myVertices.Add(FVector(0, b, -a));
    const auto v2 = mesh.myVertices.Add(FVector(b, a, 0));
    const auto v3 = mesh.myVertices.Add(FVector(-b, a, 0));
    const auto v4 = mesh.myVertices.Add(FVector(0, b, a));
    const auto v5 = mesh.myVertices.Add(FVector(0, -b, a));
    const auto v6 = mesh.myVertices.Add(FVector(-a, 0, b));
    const auto v7 = mesh.myVertices.Add(FVector(0, -b, -a));
    const auto v8 = mesh.myVertices.Add(FVector(a, 0, -b));
    const auto v9 = mesh.myVertices.Add(FVector(a, 0, b));
    const auto v10 = mesh.myVertices.Add(FVector(-a, 0, -b));
    const auto v11 = mesh.myVertices.Add(FVector(b, -a, 0));
    const auto v12 = mesh.myVertices.Add(FVector(-b, -a, 0));

    ProjectToUnitSphere(mesh);

    // add triangles
    mesh.myIndices.Append({v3, v2, v1});
    mesh.myIndices.Append({v2, v3, v4});
    mesh.myIndices.Append({v6, v5, v4});
    mesh.myIndices.Append({v5, v9, v4});
    mesh.myIndices.Append({v8, v7, v1});
    mesh.myIndices.Append({v7, v10, v1});
    mesh.myIndices.Append({v12, v11, v5});
    mesh.myIndices.Append({v11, v12, v7});
    mesh.myIndices.Append({v10, v6, v3});
    mesh.myIndices.Append({v6, v10, v12});
    mesh.myIndices.Append({v9, v8, v2});
    mesh.myIndices.Append({v8, v9, v11});
    mesh.myIndices.Append({v3, v6, v4});
    mesh.myIndices.Append({v9, v2, v4});
    mesh.myIndices.Append({v10, v3, v1});
    mesh.myIndices.Append({v2, v8, v1});
    mesh.myIndices.Append({v12, v10, v7});
    mesh.myIndices.Append({v8, v11, v7});
    mesh.myIndices.Append({v6, v12, v5});
    mesh.myIndices.Append({v11, v9, v5});

    return mesh;
}

inline SphereMesh Icosphere(const size_t aSubdivisions)
{
    auto mesh = Icosahedron();
    for (size_t i = 0; i < aSubdivisions; i++)
        ProjectToUnitSphere(mesh);
    return mesh;
}
*/