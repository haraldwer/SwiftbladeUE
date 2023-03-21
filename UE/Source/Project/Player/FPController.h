#pragma once

#include "CoreMinimal.h"
#include "FPState.h"
#include "GameFramework/PlayerController.h"
#include "Project/UI/Menus/MenuBase.h"
#include "FPController.generated.h"

class ACheckpoint;
class AGameEnd;

UCLASS()
class PROJECT_API AFPController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual ~AFPController() override = default;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	class AFPCharacter* GetFPCharacter() const;
	class UCustomCamera* GetCamera() const;

	FFPState GetState() const;
	void SetState(const FFPState& aState) const;
	void UpdateState(FFPState& aState) const;

	void CharacterKilled();
	
	UFUNCTION(BlueprintPure)
	int32 GetRespawns() const;
	UFUNCTION(BlueprintPure)
	int32 GetRemainingLives() const;
	UFUNCTION(BlueprintCallable)
	void Respawn();

	bool TrySetCheckpoint(ACheckpoint& aCheckpoint);
	void UseCheckpoint(const ACheckpoint& aCheckpoint) const;

	UFUNCTION(BlueprintCallable)
	void StartTravel(EFPTravelReason aReason);
	UFUNCTION(BlueprintCallable)
	void FinishTravel();

	void ReachEnd(const AGameEnd& aGameEnd);
	
	UFUNCTION(BlueprintCallable)
	void SetEnablePawnControls(bool aEnabled);

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int myNumSeeds = 10;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int myRespawns = 3;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float myMinAllowedTime = 10.0f;
	
private:

	// Enter the next section. Will reload level.
	void EnterSection() const;
	// Enter the next arena. Will reload level.
	void EnterArena() const;
	
	virtual void SetupInputComponent() override;
	void PausePressed();
	
	TWeakObjectPtr<ACheckpoint> myCheckpoint;

	bool myHasReachedEnd = false;
	
};
