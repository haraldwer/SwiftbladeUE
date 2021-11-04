#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hand.generated.h"

UCLASS()
class PROJECT_API AHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AHand();
	void SetIsOpen(bool aIsOpen) const;
};
