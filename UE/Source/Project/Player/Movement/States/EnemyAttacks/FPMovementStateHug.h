#pragma once

#include "CoreMinimal.h"
#include "../FPMovementStateBase.h"
#include "FPMovementStateHug.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateHug : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:

	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;
	virtual UClass* OnHit(const FHitResult& aHit) override;
	virtual UClass* OnLanded() override;

	virtual int32 Priority() const override { return 10; }
	virtual bool CanJump() const override { return false; }
	
};
