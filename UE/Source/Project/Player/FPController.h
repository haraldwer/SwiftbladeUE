#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPController.generated.h"

UCLASS()
class PROJECT_API AFPController : public APlayerController
{
	GENERATED_BODY()

public:
	
	virtual ~AFPController() override = default;

protected:
	
	virtual void BeginPlay() override;

public:

	class AFPCharacter* GetFPCharacter() const;
	
	void CharacterCreated(AFPCharacter* aCharacter);
	void CharacterKilled();
	
	UFUNCTION(BlueprintCallable)
	void Respawn();

	void EnterSection() const;
	void EnterArena() const;

	bool SetCheckpoint(class ACheckpoint* aCheckpoint);

	void SetEnablePawnControls(bool aEnabled);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<AFPCharacter> myCharacterBlueprint;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int myRespawns = 2;
	
private:
	
	virtual void SetupInputComponent() override;
	
	void PausePressed();

	TWeakObjectPtr<ACheckpoint> myCheckpoint;
	int32 myArenaIndex = 0;
	FVector myStartLocation;
	int myRespawnCount = 0;
};
