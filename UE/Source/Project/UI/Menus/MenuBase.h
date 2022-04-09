#pragma once

#include "CoreMinimal.h"
#include "MenuManager.h"
#include "Project/UI/Widgets/WidgetBase.h"
#include "MenuBase.generated.h"

USTRUCT(BlueprintType)
struct FMenuInputMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool myLockMouseToViewport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool myHideCursor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool myAllowPlayerControl;
};

UCLASS()
class PROJECT_API UMenuBase : public UWidgetBase
{
	GENERATED_BODY()
	
public:
	
	virtual ~UMenuBase() override = default;
	void Init(EMenuType aMenuType);

	UFUNCTION(BlueprintImplementableEvent)
	void OnOpen();
	UFUNCTION(BlueprintImplementableEvent)
	void OnClose();

	UFUNCTION(BlueprintCallable)
	EMenuType GetMenuType() const { return myMenuType; }
	UFUNCTION(BlueprintCallable)
	FMenuInputMode GetInputMode() const { return myInputMode; }
	UFUNCTION(BlueprintCallable)
	virtual int GetZOrder() const override { return myZOrder; }
	UFUNCTION(BlueprintCallable)
	bool GetIsExclusive() const { return myExclusive; }
	
protected:

	UPROPERTY(EditDefaultsOnly)
	FMenuInputMode myInputMode;

	UPROPERTY(EditDefaultsOnly)
	int myZOrder = 0;

	UPROPERTY(EditDefaultsOnly)
	bool myExclusive = false;
	
private:
	
	UPROPERTY(EditDefaultsOnly)
	EMenuType myMenuType = EMenuType::UNKNOWN;
};
