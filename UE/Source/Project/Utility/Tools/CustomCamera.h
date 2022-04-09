#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CustomCamera.generated.h"

class UWidgetBase;

USTRUCT()
struct FWidgetEntry
{
	GENERATED_BODY()
	
	TWeakObjectPtr<UWidgetBase> myWidget;
	TWeakObjectPtr<class UCustomWidgetComponent> myComponent;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UCustomCamera : public UCameraComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void AddWidget(UWidgetBase* aWidget, int32 aZOrder);
	void RemoveWidget(const UWidgetBase* aWidget);
	
protected:
	void RefreshComponents();
	
	UCustomWidgetComponent* GetWidgetComponent();
	void ReturnWidgetComponent(UCustomWidgetComponent* aComp);
	
private:
	TArray<FWidgetEntry> myWidgets;
	TArray<UCustomWidgetComponent*> myUnusedComponents;
};
