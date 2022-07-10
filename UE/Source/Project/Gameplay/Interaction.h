#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Project/Player/Actors/HandStateEnum.h"
#include "Interaction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class AFPCharacter*, aCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class AFPCharacter*, aCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformInteraction, class AFPCharacter*, aCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickInteraction, class AFPCharacter*, aCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickInteraction, class AFPCharacter*, aCharacter);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UInteractionHand : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UInteractionHand();
	EHandState GetHandState() const { return myHandState; }
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
	EHandState myHandState = EHandState::OPEN;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UInteraction : public USceneComponent
{
	GENERATED_BODY()

public:
	UInteraction();
	virtual void BeginPlay() override;

	virtual void BeginInteract(class AFPCharacter* aCharacter);
	virtual void EndInteract(class AFPCharacter* aCharacter);
	virtual bool TickInteraction(class AFPCharacter* aCharacter);
	virtual void PerformInteraction(class AFPCharacter* aCharacter);
	virtual void ClickInteraction(class AFPCharacter* aCharacter);

	UFUNCTION(BlueprintCallable)
	void FinishInteraction();

	UFUNCTION(BlueprintPure)
	bool HasFinished() const { return myFinished; }

	UFUNCTION(BlueprintPure)
	bool IsInteracting() const { return myInteracting; }

	bool GetHand(int32 anIndex, FTransform& aTrans, EHandState& aHandState);
	bool GetLeft(FTransform& aTrans, EHandState& aHandState) { return GetHand(0, aTrans, aHandState); }
	bool GetRight(FTransform& aTrans, EHandState& aHandState) { return GetHand(1, aTrans, aHandState); }

protected:
	
	UPROPERTY(BlueprintAssignable)
	FOnBeginInteract myOnBeginInteract;

	UPROPERTY(BlueprintAssignable)
	FOnEndInteract myOnEndInteract;

	UPROPERTY(BlueprintAssignable)
	FOnTickInteraction myOnTickInteraction;

	UPROPERTY(BlueprintAssignable)
	FOnPerformInteraction myOnPerformInteraction;

	UPROPERTY(BlueprintAssignable)
	FOnClickInteraction myOnClickInteraction;
	
private:

	UPROPERTY()
	TArray<UInteractionHand*> myHandComponents;
	
	bool myFinished = false;
	bool myInteracting = false;
};
