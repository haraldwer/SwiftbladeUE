#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction.generated.h"

UCLASS()
class PROJECT_API AInteraction : public AActor
{
	GENERATED_BODY()

public:
	AInteraction();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* myCollider;
};
