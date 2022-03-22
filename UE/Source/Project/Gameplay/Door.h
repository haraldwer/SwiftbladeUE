#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class AFPCharacter;

UCLASS()
class PROJECT_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:
	ADoor();
	
	// Open door
	void Open(AFPCharacter* anInstigator);

	// When door started opening
	UFUNCTION(BlueprintImplementableEvent)
	void OnOpen();

	// When door completely opened
	UFUNCTION(BlueprintImplementableEvent)
	void OnOpened();

	UFUNCTION(BlueprintCallable)
	void NotifyOpen();
	
private:
	TWeakObjectPtr<AFPCharacter> myInstigator;
};
