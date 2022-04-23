#pragma once

#include "CoreMinimal.h"
#include "HandStateEnum.h"
#include "GameFramework/Actor.h"
#include "Hand.generated.h"

UCLASS()
class PROJECT_API AHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AHand();
	void SetHandState(EHandState aState) const;
};
