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
	UCustomCamera();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
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
