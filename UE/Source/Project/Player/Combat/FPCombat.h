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
	virtual bool SetState(UStateBase* aState) override;
	
	// Sword
	void PickupSword();
	void ReturnSword();
	void SetHasSword(bool aValue);
	bool HasSword() const;
	void SetSword(class ASword* aSword);
	class ASword* GetSword() const;

	// Input
	void Strike()	{ Input(EFPCombatInput::STRIKE);	}
	void Block()	{ Input(EFPCombatInput::BLOCK);		}
	void Interact() { Input(EFPCombatInput::INTERACT);	}
	
	bool TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser) const;

protected:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASword> mySwordBP;
	
private:

	class AFPCharacter& GetCharacter() const;
	void Input(EFPCombatInput anInput);
	void TryOverrideAnimation() const;
	
	TWeakObjectPtr<ASword> mySword;
	bool mySwordFirstTick = false;
	
};
