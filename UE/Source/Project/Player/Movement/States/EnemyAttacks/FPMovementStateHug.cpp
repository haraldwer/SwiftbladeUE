#include "FPMovementStateHug.h"

#include "Project/Player/Movement/FPMovement.h"

UClass* UFPMovementStateHug::Update(float aDT)
{
	return StaticClass(); 
}

UClass* UFPMovementStateHug::Input(EFPMovementInputAction anAction, float aValue)
{
	return IsCurrentState() ? StaticClass() : nullptr;
}

UClass* UFPMovementStateHug::OnHit(const FHitResult& aHit)
{
	return IsCurrentState() ? StaticClass() : nullptr;
}

UClass* UFPMovementStateHug::OnLanded()
{
	return IsCurrentState() ? StaticClass() : nullptr;
}
