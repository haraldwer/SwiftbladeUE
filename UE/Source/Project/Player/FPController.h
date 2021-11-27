#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPController.generated.h"

UCLASS()
class PROJECT_API AFPController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void SetEnablePawnControls(bool aEnabled);

private:

	virtual void SetupInputComponent() override;
	void PausePressed();

	UPROPERTY()
	class AFPCharacter* myCharacter;
};
