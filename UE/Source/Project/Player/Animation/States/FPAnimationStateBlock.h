#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateBlock.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateBlock : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	virtual UClass* Update(float aDT) override;
	virtual void Enter() override;
	virtual int32 Priority() const override { return 1; }

	void OnBlock() const;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleStrength = 2.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleSpeed = 1.0f;
	
private:

	int32 myAnimIndex = 0;
	int32 myPreviousIndex = 0;
	TArray<TWeakObjectPtr<USceneComponent>> myEntries;
};
