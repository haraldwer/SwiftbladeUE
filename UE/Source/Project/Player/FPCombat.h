#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPCombat.generated.h"

UENUM(BlueprintType)
enum class EFPCombatState : uint8 {
	NO_SWORD	UMETA(DisplayName = "NO_SWORD"),
	IDLE		UMETA(DisplayName = "IDLE"),
	READY		UMETA(DisplayName = "READY"),
	STRIKE		UMETA(DisplayName = "STRIKE"),
	DEFLECT 	UMETA(DisplayName = "DEFLECT")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPCombat : public UActorComponent
{
	GENERATED_BODY()

public:	

	
	
	UFPCombat();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void PickupSword();
	bool GetHasSword() const;
	
	void Strike();

	void UpdateStrike();
	void UpdateSword(float aDT);
	
	void UpdateTransforms(float aDT);
	
	class ASword* GetSword() const;

private:
	
	class AFPCharacter* GetFPCharacter() const;
	
	static FTransform LerpTrans(const FTransform& aFirst, const FTransform& aSecond, float aLocationWeight, float aRotationWeight);
	static FTransform DTLerpTrans(const FTransform& aFirst, const FTransform& aSecond, float aDT, float aSmoothing);
	
protected:
	
	UPROPERTY()
	class ASword* mySword;

	UPROPERTY(EditDefaultsOnly)
	float mySmoothing = 15.0f;

private:

	bool myUseBothHands = false;
	bool myUseHandRot = true;
	bool myUseWeight = false;
	
	float myTargetLocationWeight = 0.0f;
	float myLocationWeight = 0.0f;

	float myTargetRotationWeight = 0.0f;
	float myRotationWeight = 0.0f;
	
	FTransform myTargetTrans;
	FTransform myTrans;
	
	float myStrikeTime = 9999.0f;

	EFPCombatState myState = EFPCombatState::NO_SWORD;
};
