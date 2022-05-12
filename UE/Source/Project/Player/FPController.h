#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPController.generated.h"

USTRUCT(BlueprintType)
struct FFPControllerState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 myArenaIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool myInArena = false; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 myRespawnCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 mySeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool myHasSword = false;
	
};

UCLASS()
class PROJECT_API AFPController : public APlayerController
{
	GENERATED_BODY()

public:
	
	virtual ~AFPController() override = default;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	class AFPCharacter* GetFPCharacter() const;

	// -- State -- //
	
	UFUNCTION(BlueprintImplementableEvent)
	void LoadState();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SaveState();

	UFUNCTION(BlueprintCallable)
	void OnStateLoaded(FFPControllerState aState);

	UFUNCTION(BlueprintCallable)
	FFPControllerState GetState() const;
	
	// -- End state -- // 

	// Called when character dies
	void CharacterKilled();

	// Called when it's time to respawn
	UFUNCTION(BlueprintCallable)
	void Respawn();

	// Enter the next section. Will reload level.
	void EnterSection();
	// Enter the next arena. Will reload level.
	void EnterArena();

	// Called when checkpoint is set
	bool SetCheckpoint(class ACheckpoint* aCheckpoint);
	void TravelCheckpoint(const class ACheckpoint* aCheckpoint);

	// UI helper function
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
	FFPControllerState myState; 
};
