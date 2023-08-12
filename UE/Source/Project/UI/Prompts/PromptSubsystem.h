#pragma once

#include "CoreMinimal.h"
#include "PromptSubsystem.generated.h"

class UPromptBase;

UENUM(BlueprintType)
enum class EPromptType : uint8
{
	UNKNOWN,
	DEATH,
	INTRO,
	OUTRO,
	LEVEL_END,
	CHECKPOINT,
	LEVEL_FADEOUT,
	LEVEL_FADEIN,
	COUNT
};

UCLASS()
class PROJECT_API UPromptSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	static UPromptSubsystem& Get();
	
	UFUNCTION(BlueprintCallable)
	UPromptBase* CreatePrompt(EPromptType aPrompt);
	UFUNCTION(BlueprintCallable)
	void DestroyPrompt(EPromptType aPrompt);

	UFUNCTION(BlueprintCallable)
	bool IsPromptOpen(EPromptType aPrompt) const;
	UFUNCTION(BlueprintCallable)
	bool IsAnyPromptOpen() const;

protected:
	UPromptBase* GetPromptInstance(EPromptType aPrompt);
	
	UPROPERTY(EditDefaultsOnly)
	TMap<EPromptType, TSubclassOf<UPromptBase>> myPromptBlueprints;

	UPROPERTY()
	TArray<UPromptBase*> myOpenPrompts;

	UPROPERTY()
	TArray<UPromptBase*> myPrompts;
};
