#pragma once

#include "CoreMinimal.h"
#include "FPState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FPStateSubsystem.generated.h"

UCLASS()
class PROJECT_API UFPStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class AFPController;

public: 
	
	void Set(const FFPState& aState) { myState = aState; }
	const FFPState& Get() const { return myState; }

private: 
	
	FFPState myState; 
	
};
