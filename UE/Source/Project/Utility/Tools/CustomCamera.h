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
	TWeakObjectPtr<class UWidgetComponent> myComponent;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UCustomCamera : public UCameraComponent
{
	GENERATED_BODY()

public:
	UCustomCamera();
	virtual void BeginPlay() override;
	
	class UWidgetInteractionComponent* GetWidgetInteraction() const	{ return myWidgetInteractionComponent; }

	void AddWidget(UWidgetBase* aWidget, int32 aZOrder);
	void RemoveWidget(UWidgetBase* aWidget);
	
protected:
	
	UWidgetComponent* GetWidgetComponent();
	void ReturnWidgetComponent(UWidgetComponent* aComp);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UWidgetInteractionComponent* myWidgetInteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TArray<UWidgetComponent*> myWidgetComponents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	FTransform myWidgetTransform;

private:
	TArray<FWidgetEntry> myWidgets;
	TArray<UWidgetComponent*> myUnusedComponents;
};
