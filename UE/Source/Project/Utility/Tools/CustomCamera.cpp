#include "CustomCamera.h"
#include "../Utility.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Project/UI/Widgets/CustomWidgetComponent.h"
#include "Project/UI/Widgets/WidgetBase.h"

void UCustomCamera::BeginPlay()
{
	Super::BeginPlay();
	RefreshComponents();
}

void UCustomCamera::AddWidget(UWidgetBase* aWidget, const int32 aZOrder)
{	
	CHECK_RETURN_LOG(!aWidget, "Invalid ptr");
	const auto find = myWidgets.FindByPredicate([&](const FWidgetEntry& anEntry)
	{
		return anEntry.myWidget.Get() == aWidget;
	});
	CHECK_RETURN_LOG(find, "Widget already added to camera");
	const auto comp = GetWidgetComponent();
	CHECK_RETURN_LOG(!comp, "Failed to create widget component");
	myWidgets.Emplace(FWidgetEntry{aWidget, comp});
	comp->SetInitialLayerZOrder(aZOrder);
	comp->SetWidgetClass(aWidget->GetClass());
	comp->SetWidget(aWidget);
	comp->SetVisibility(true);
}

void UCustomCamera::RemoveWidget(const UWidgetBase* aWidget)
{
	CHECK_RETURN_LOG(!aWidget, "Invalid ptr");
	for (int i = 0; i < myWidgets.Num(); i++)
	{
		auto& widget = myWidgets[i];
		CHECK_CONTINUE(widget.myWidget.Get() != aWidget);
		// Found widget
		ReturnWidgetComponent(widget.myComponent.Get());
		widget.myComponent.Reset();
		myWidgets.RemoveAt(i);
		return;
	}
	LOG("Widget does not exist on camera");
}

void UCustomCamera::RefreshComponents()
{
	TArray<USceneComponent*> children;
	GetChildrenComponents(true, children);
	for (const auto c : children)
	{
		// Get widget components
		if (auto widgetComponent = Cast<UCustomWidgetComponent>(c))
		{
			widgetComponent->SetActive(false);
			myUnusedComponents.Add(widgetComponent);
		}
	}
}

UCustomWidgetComponent* UCustomCamera::GetWidgetComponent()
{
	CHECK_RETURN_LOG(myUnusedComponents.Num() <= 0, "No unused components", nullptr);
	const auto comp = myUnusedComponents.Last();
	comp->SetActive(true);
	myUnusedComponents.RemoveAt(myUnusedComponents.Num() - 1);
	return comp;
}

void UCustomCamera::ReturnWidgetComponent(UCustomWidgetComponent* aComp)
{
	CHECK_RETURN_LOG(!aComp, "Tried to return null comp");
	CHECK_RETURN_LOG(myUnusedComponents.Contains(aComp), "Component already returned")
	aComp->SetWidgetClass(nullptr);
	aComp->SetWidget(nullptr);
	aComp->SetActive(false);
	aComp->SetInitialLayerZOrder(0);
	aComp->SetVisibility(false);
	myUnusedComponents.Add(aComp);
}

