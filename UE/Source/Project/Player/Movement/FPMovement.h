#pragma once

#include "CoreMinimal.h"
#include "FPMovementInputAction.h"
#include "Project/Utility/Tools/StateMachine/StateMachine.h"
#include "FPMovement.generated.h"

UCLASS(ClassGroup=(Custom))
class PROJECT_API UFPMovement : public UStateMachine
{
	GENERATED_BODY()

public:
	virtual ~UFPMovement() override = default;
	virtual UClass* GetDefaultStateType() override;

	void MoveHorizontal(const float aValue) 	{ Input(EFPMovementInputAction::MOVE_HORIZONTAL, aValue, false); }
	void MoveVertical(const float aValue) 		{ Input(EFPMovementInputAction::MOVE_VERTICAL, aValue, false); }
	void LookHorizontal(const float aValue) 	{ Input(EFPMovementInputAction::LOOK_HORIZONTAL, aValue, false); }
	void LookVertical(const float aValue) 		{ Input(EFPMovementInputAction::LOOK_VERTICAL, aValue, false); }
	void JumpPressed() 							{ Input(EFPMovementInputAction::JUMP_PRESSED); }
	void JumpReleased() 						{ Input(EFPMovementInputAction::JUMP_RELEASED); }
	void CrouchPressed() 						{ Input(EFPMovementInputAction::CROUCH_PRESSED); }
	void CrouchReleased() 						{ Input(EFPMovementInputAction::CROUCH_RELEASED); }
	void Dash() 								{ Input(EFPMovementInputAction::DASH); }
	void Grapple() 								{ Input(EFPMovementInputAction::GRAPPLE); }

	void Input(const EFPMovementInputAction& anAction, float aValue = 0.0f, bool aAll = true);

	void OnLanded();
	void OnHit(const FHitResult& aHit);
};