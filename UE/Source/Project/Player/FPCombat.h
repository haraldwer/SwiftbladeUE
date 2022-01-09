#pragma once

#include "CoreMinimal.h"
#include "FPComponentBase.h"
#include "FPCombat.generated.h"

UENUM(BlueprintType)
enum class EFPCombatState : uint8 {
	NO_SWORD	UMETA(DisplayName = "NO_SWORD"),
	IDLE		UMETA(DisplayName = "IDLE"),
	READY		UMETA(DisplayName = "READY"),
	STRIKE		UMETA(DisplayName = "STRIKE"),
	DEFLECT 	UMETA(DisplayName = "DEFLECT")
};

USTRUCT()
struct FSwordAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FTransform myReadyTransform;

	UPROPERTY(EditDefaultsOnly)
	FTransform myStrikeTransform;
	
	UPROPERTY(EditDefaultsOnly)
	FTransform myDeflectTransform;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPCombat : public UFPComponentBase
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

	void SetTransforms();
	void UpdateTransforms(float aDT);
	
	class ASword* GetSword() const;

private:
	
	static FTransform LerpTrans(const FTransform& aFirst, const FTransform& aSecond, float aLocationWeight, float aRotationWeight);
	static FTransform DTLerpTrans(const FTransform& aFirst, const FTransform& aSecond, float aDT, float aSmoothing);
	
	void HitCheckpoint(AActor* aCheckpoint) const;
	void HitEnemy(AActor* anEnemy);
	
protected:
	
	UPROPERTY()
	class ASword* mySword;

	UPROPERTY(EditDefaultsOnly)
	float mySmoothing = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float myReadyDuration = 0.5f;
	
	UPROPERTY(EditDefaultsOnly)
	FTransform myIdleSwordTransform;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FSwordAnimationData> myAnimations;

	UPROPERTY(EditDefaultsOnly)
	int32 myDefaultAnimIndex = -1;

	UPROPERTY(EditDefaultsOnly)
	float myStrikeDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myDeflectDuration = 0.5f;
	
private:

	bool myUseBothHands = false;
	bool myUseHandRot = true;
	bool myUseWeight = false;
	
	float myTargetLocationWeight = 0.0f;
	float myLocationWeight = 0.0f;
	float myLeftWeight = 0.0f;

	float myTargetRotationWeight = 0.0f;
	float myRotationWeight = 0.0f;
	
	FTransform myTargetTrans;
	FTransform myTrans;
	
	float myStrikeTime = 9999.0f;
	int32 myAnimIndex = 0;

	EFPCombatState myState = EFPCombatState::NO_SWORD;
	
};
