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
	float myTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool myHasSword = false;
};

UENUM()
enum class EFPTravelReason
{
	NONE,
	RESPAWN,
	CHECKPOINT,
	ARENA,
	COUNT
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
	class UCustomCamera* GetCamera() const;

	// -- State -- //

	UFUNCTION(BlueprintImplementableEvent)
	void LoadState();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SaveState();
	
	UFUNCTION(BlueprintCallable)
	void OnStateLoaded(FFPControllerState aState);

	UFUNCTION(BlueprintCallable)
	FFPControllerState GetState() const;

	// -- Respawning -- // 
	
	// Called when character dies
	void CharacterKilled();

	// Called when it's time to respawn
	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION(BlueprintPure)
	int32 GetRemainingLives() const { return myRespawns - myState.myRespawnCount; }

	UFUNCTION(BlueprintPure)
	int32 GetRespawns() const { return myRespawns; } 
	
	// -- Checkpoints -- // 
	
	// Called when checkpoint is set
	bool TrySetCheckpoint(class ACheckpoint* aCheckpoint);
	void UseCheckpoint(const class ACheckpoint* aCheckpoint);

	// -- Travelling -- //
	
	void StartTravel(EFPTravelReason aReason);
	
	UFUNCTION(BlueprintCallable)
	void FinishTravel();

	// -- Reaching the end -- //

	void ReachEnd(class AGameEnd* aGameEnd);
	
	// -- Other -- // 
	
	// UI helper function
	UFUNCTION(BlueprintCallable)
	void SetEnablePawnControls(bool aEnabled);
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int myRespawns = 3;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int myMinAllowedTime = 10.0f;
	
private:

	// Enter the next section. Will reload level.
	void EnterSection();
	// Enter the next arena. Will reload level.
	void EnterArena();
	
	virtual void SetupInputComponent() override;
	void PausePressed();
	
	TWeakObjectPtr<ACheckpoint> myCheckpoint;
	FFPControllerState myState;
	EFPTravelReason myTravelReason = EFPTravelReason::NONE;

	bool myHasReachedEnd = false;
};
