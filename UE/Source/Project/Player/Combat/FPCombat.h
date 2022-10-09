#pragma once

#include "CoreMinimal.h"
#include "FPCombatInputActions.h"
#include "Project/Utility/Tools/StateMachine/StateMachine.h"
#include "FPCombat.generated.h"

UCLASS( ClassGroup=(Custom) )
class PROJECT_API UFPCombat : public UStateMachine
{
	GENERATED_BODY()

public:
	
	UFPCombat();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual UClass* GetDefaultStateType() override;
	virtual bool SetStatePtr(UStateBase* aState) override;
	
	// Sword
	void PickupSword();
	void ReturnSword();
	void SetHasSword(bool aValue);
	bool HasSword() const;
	void SetSword(class ASword* aSword);
	class ASword* GetSword() const;

	// Input
	void Strike()	{ QueueInput(EFPCombatInput::STRIKE);	}
	void Block()	{ QueueInput(EFPCombatInput::BLOCK);		}
	void Interact() { QueueInput(EFPCombatInput::INTERACT);	}
	
	bool TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser) const;

protected:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASword> mySwordBP;

	UPROPERTY(EditDefaultsOnly)
	float myInputQueueTime = 0.5f;
	
private:
	
	class AFPCharacter& GetCharacter() const;
	
	void QueueInput(EFPCombatInput anInput);
	
	void UpdateInput();
	bool CheckInput(EFPCombatInput anInput);
	
	void TryOverrideAnimation() const;
	
	TWeakObjectPtr<ASword> mySword;
	bool mySwordFirstTick = false;

	TMap<uint8, float> myInputQueue; 
	
};
