#pragma once

#include "CoreMinimal.h"
#include "PromptSubsystem.h"
#include "Project/UI/Widgets/WidgetBase.h"
#include "PromptBase.generated.h"

UCLASS()
class PROJECT_API UPromptBase : public UWidgetBase
{
	GENERATED_BODY()
	
public:
	
	virtual ~UPromptBase() override = default;
	void Init(EPromptType aPrompt);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCreate();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroy();

	UFUNCTION(BlueprintCallable)
	void Destroy();
	
	UFUNCTION(BlueprintCallable)
	EPromptType GetPromptType() const { return myPromptType; }
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
	EPromptType myPromptType = EPromptType::UNKNOWN;

};
