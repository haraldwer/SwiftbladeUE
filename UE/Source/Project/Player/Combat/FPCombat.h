#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "Project/Player/FPComponentBase.h"
#include "FPCombat.generated.h"

UENUM(BlueprintType)
enum class EFPCombatState : uint8 {
	NO_SWORD,
	IDLE,
	STRIKE,	
	BLOCK
};

USTRUCT()
struct FSwordAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FTransform myStart;

	UPROPERTY(EditDefaultsOnly)
	FTransform myEnd;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPCombat : public UFPActorComponentBase
{
	GENERATED_BODY()

public:
	
	UFPCombat();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void PickupSword();
	void ReturnSword();
	void SetHasSword(bool aValue);
	bool GetHasSword() const;
	
	void Strike();
	void Block();
	void Interact();

	void Update(float aDT);
	
	class ASword* GetSword() const;

private:

	void DoStrike();
	void DoBlock();
	void DoInteract();
	
	void HitCheckpoint(AActor* aCheckpoint) const;
	void HitEnemy(AActor* anEnemy);
	void SelectAnim(int aNum);
	
protected:
	
	UPROPERTY()
	class ASword* mySword;

	UPROPERTY(EditDefaultsOnly)
	float mySmoothing = 30.0f;
	
	UPROPERTY(EditDefaultsOnly)
	FTransform myIdleSwordTransform;

	UPROPERTY(EditDefaultsOnly)
	float myInteractDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEffect> myHitEffectBP;

	UPROPERTY(EditDefaultsOnly)
	TArray<FSwordAnimationData> myStrikeAnimations;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FSwordAnimationData> myBlockAnimations;

	UPROPERTY(EditDefaultsOnly)
	float myStrikeDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myPerformStrikeDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* myStrikeAnimationCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* myStrikeAnimationWeightCurve;

	UPROPERTY(EditDefaultsOnly)
	float myBlockDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myPerformBlockDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* myBlockAnimationCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* myBlockAnimationWeightCurve;

private:
	
	float myTimer = 9999.0f;
	int32 myAnimIndex = 0;
	int32 myPreviousAnimIndex = -1;

	EFPCombatState myState = EFPCombatState::NO_SWORD;
};

