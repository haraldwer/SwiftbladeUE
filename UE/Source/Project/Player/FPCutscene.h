#pragma once

#include "CoreMinimal.h"
#include "FPComponentBase.h"
#include "Components/ActorComponent.h"
#include "FPCutscene.generated.h"

UENUM(BlueprintType)
enum EFPCutsceneType
{
	INTRO,
	ENTER_ARENA,
	EXIT_ARENA,
	OUTRO
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCutsceneStarted, EFPCutsceneType, aCutsceneType);

UCLASS(ClassGroup=(Custom))
class PROJECT_API UFPCutscene : public UFPActorComponentBase
{
	GENERATED_BODY()

	UFPCutscene();

public:
	
	void StartCutscene(EFPCutsceneType aType);

	UPROPERTY(BlueprintAssignable)
	FOnCutsceneStarted myOnCutsceneStarted;
	
};
