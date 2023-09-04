#include "WaveFunctionCollapse/Example/WFCStructureGrid.h"

void AWFCStructureGrid::CreateStructure()
{
	const int32 num = myGridSize * myGridSize;
	for (int32 i = 0; i < num; i++)
	{
		FVector loc =
			FVector::RightVector * GetX(i) +
			FVector::ForwardVector * GetY(i); 
		CreateNode(FTransform(loc * myScale));
	}
}

TArray<int32> AWFCStructureGrid::GetNeighbours(const int32 anIndex) const
{
	// up down left right
	const int32 x = GetX(anIndex);
	const int32 y = GetY(anIndex);
	
	TArray<int32> result;
	for (int32 w = -1; w <= 1; w++)
	{
		if (x + w >= myGridSize || x + w < 0)
			continue;
		for (int32 h = -1; h <= 1; h++)
		{
			if (y + h >= myGridSize || y + h < 0)
				continue;
			result.Add(GetIndex(x + w, y + h));
		}
	}
	return result; 
}

int32 AWFCStructureGrid::GetIndex(const int32 anX, const int32 aY) const
{
	return aY * myGridSize + anX; 
}

int32 AWFCStructureGrid::GetX(const int32 anIndex) const
{
	return anIndex % myGridSize;
}

int32 AWFCStructureGrid::GetY(const int32 anIndex) const
{
	return anIndex / myGridSize; 
}
