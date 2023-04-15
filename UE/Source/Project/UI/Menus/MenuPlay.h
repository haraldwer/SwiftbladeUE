#pragma once

#include "MenuBase.h"
#include "MenuPlay.generated.h"

UCLASS()
class PROJECT_API UMenuPlay : public UMenuBase
{
	GENERATED_BODY()
	
public:
 
	UFUNCTION(BlueprintCallable)
	void SetState(int32 InChapter, int32 InSeed);
	 
};
