#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetBase.generated.h"

UCLASS()
class PROJECT_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual ~UWidgetBase() override = default;
	
	virtual void NativeConstruct() override;
};
