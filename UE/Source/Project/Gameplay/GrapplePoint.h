#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplePoint.generated.h"

UCLASS()
class PROJECT_API AGrapplePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrapplePoint();
	void SetFocus(bool aFocused);
	
	UFUNCTION(BlueprintPure)
	bool IsFocused() const { return myFocused; }
	
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginFocus();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndFocus();
	
private:
	
	bool myFocused = false;
	
};
