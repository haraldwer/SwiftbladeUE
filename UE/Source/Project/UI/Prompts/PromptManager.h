#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PromptManager.generated.h"

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
class PROJECT_API APromptManager : public AActor
{
	GENERATED_BODY()

public:
	APromptManager();
	virtual ~APromptManager() override = default;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	class UPromptBase* CreatePrompt(EPromptType aPrompt);
	UFUNCTION(BlueprintCallable)
	void DestroyPrompt(EPromptType aPrompt);

	UFUNCTION(BlueprintCallable)
	bool IsPromptOpen(EPromptType aPrompt) const;
	UFUNCTION(BlueprintCallable)
	bool IsAnyPromptOpen() const;

protected:
	UPromptBase* GetPromptInstance(EPromptType aPrompt);
	
	UPROPERTY(EditDefaultsOnly)
	TMap<EPromptType, TSubclassOf<class UPromptBase>> myPromptBlueprints;

	UPROPERTY()
	TArray<UPromptBase*> myOpenPrompts;

	UPROPERTY()
	TArray<UPromptBase*> myPrompts;
};
